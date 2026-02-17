/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "Bundle.h"
#include "../../Brain.h"
#include "Definitions/SubFixture/SubFixture.h"
#include "BundleSelectionManager.h"
#include "BundleSelection.h"
#include "UserInputManager.h"
#include "BundleManager.h"
#include "Definitions/ChannelFamily/ChannelFamilyManager.h"

Bundle::Bundle(var params) :
	BaseItem(params.getProperty("name", "Bundle")),
	objectType(params.getProperty("type", "Bundle").toString()),
	objectData(params)
{
	saveAndLoadRecursiveData = true;
	editorIsCollapsed = false;
	itemDataType = "Bundle";
	nameCanBeChangedByUser = false;

	id = addIntParameter("ID", "ID of this bundle", 1, 1);
	userName = addStringParameter("Name", "Name of this bundle", "New bundle");
	updateName();

	// to add a manager with defined data
	//selection = new BundleSelectionManager();
	addChildControllableContainer(&selection);

	if (params.isVoid()) {
		selection.addItem();
	}

	Brain::getInstance()->registerBundle(this, id->getValue());
	updateDisplay();
	//listeners.clear();

	//Brain::getInstance()->allBundles.add(this);

}

Bundle::~Bundle()
{
}

void Bundle::updateDisplay() {
	// valueTo -> hideInEditor = !(bool)thru->getValue();
}

void Bundle::updateName()
{
	String n = userName->getValue();
	if (parentContainer != nullptr) {
		dynamic_cast<BundleManager*>(parentContainer.get())->reorderItems();
	}
	setNiceName(String((int)id->getValue()) + " - " + n);
}

void Bundle::onContainerParameterChangedInternal(Parameter* p)
{
	if (p == id) {
		Brain::getInstance()->registerBundle(this, id->getValue());
	}
	if (p == userName || p == id) {
		updateName();
	}

}

void Bundle::computeValues() {

	if (TSLastUpdate > Brain::getInstance()->TSBundles) return;

	isComputing.enter();
	computedCuelists.clear();
	computedEffects.clear();
	computedCarousels.clear();
	computedMappers.clear();
	computedTrackers.clear();

	for (BundleSelection* s : selection.items) {
		bool add = s->plusOrMinus->getValueData() == "add";
		int from = s->valueFrom->intValue();
		int to = s->thru->boolValue() ? s->valueTo->intValue() : s->valueFrom->intValue();
		int delta = to >= from ? 1 : -1;
		String type = s->targetType->getValueData();

		for (int i = from; i <= to; i += delta) {
			if (type == "cuelist") {
				Cuelist* elmt = Brain::getInstance()->getCuelistById(i);
				if (elmt != nullptr) {
					if (add) computedCuelists.addIfNotAlreadyThere(elmt);
					else computedCuelists.removeAllInstancesOf(elmt);
				}
			}
			else if (type == "effect") {
				Effect* elmt = Brain::getInstance()->getEffectById(i);
				if (elmt != nullptr) {
					if (add) computedEffects.addIfNotAlreadyThere(elmt);
					else computedEffects.removeAllInstancesOf(elmt);
				}
			}
			else if (type == "carousel") {
				Carousel* elmt = Brain::getInstance()->getCarouselById(i);
				if (elmt != nullptr) {
					if (add) computedCarousels.addIfNotAlreadyThere(elmt);
					else computedCarousels.removeAllInstancesOf(elmt);
				}
			}
			else if (type == "mapper") {
				Mapper* elmt = Brain::getInstance()->getMapperById(i);
				if (elmt != nullptr) {
					if (add) computedMappers.addIfNotAlreadyThere(elmt);
					else computedMappers.removeAllInstancesOf(elmt);
				}
			}
			else if (type == "tracker") {
				Tracker* elmt = Brain::getInstance()->getTrackerById(i);
				if (elmt != nullptr) {
					if (add) computedTrackers.addIfNotAlreadyThere(elmt);
					else computedTrackers.removeAllInstancesOf(elmt);
				}
			}
		}
	}

	isComputing.exit();
	TSLastUpdate = Time::getMillisecondCounterHiRes();
}

void Bundle::start(float forcedFade)
{
	computeValues();
	isComputing.enter();
	for (Cuelist* c : computedCuelists) c->go(0, forcedFade);
	for (Effect* c : computedEffects) c->start(forcedFade);
	for (Carousel* c : computedCarousels) c->start(forcedFade);
	for (Mapper* c : computedMappers) c->start();
	for (Tracker* c : computedTrackers) c->start();
	isComputing.exit();
}

void Bundle::stop(float forcedFade)
{
	computeValues();
	isComputing.enter();
	for (Cuelist* c : computedCuelists) c->off(0, forcedFade);
	for (Effect* c : computedEffects) c->stop(forcedFade);
	for (Carousel* c : computedCarousels) c->stop(forcedFade);
	for (Mapper* c : computedMappers) c->stop();
	for (Tracker* c : computedTrackers) c->stop();
	isComputing.exit();
}

void Bundle::tapTempo()
{
	computeValues();
	isComputing.enter();
	for (Cuelist* c : computedCuelists) c->tapTempo();
	for (Effect* c : computedEffects) c->tapTempo();
	for (Carousel* c : computedCarousels) c->tapTempo();
	isComputing.exit();
}

void Bundle::setSize(float val, bool size, bool HTP, bool LTP, bool flash)
{
	if (HTP) lastHTP = val;
	if (LTP) lastLTP = val;
	if (size) lastSize = val;
	if (flash) lastFlash = val;

	Brain::getInstance()->virtualFadersNeedUpdate = true;

	computeValues();
	isComputing.enter();
	for (Cuelist* c : computedCuelists) {
		if (HTP) c->HTPLevel->setValue(val);
		if (LTP) c->LTPLevel->setValue(val);
		if (flash) c->flashLevel->setValue(val);
	}
	for (Effect* c : computedEffects) {
		if (size) c->sizeValue->setValue(val);
		if (flash) c->flashValue->setValue(val);
	}
	for (Carousel* c : computedCarousels) {
		if (size) c->sizeValue->setValue(val);
		if (flash) c->flashValue->setValue(val);
	}
	for (Mapper* c : computedMappers) {
		if (size) c->sizeValue->setValue(val);
	}
	for (Tracker* c : computedTrackers) {
		if (size) c->sizeValue->setValue(val);
	}
	isComputing.exit();
}

void Bundle::setSpeed(float val, bool tapTempoDivide)
{
	lastSpeed = val;
	Brain::getInstance()->virtualFadersNeedUpdate = true;

	computeValues();
	isComputing.enter();
	for (Cuelist* c : computedCuelists) {
		double mult = 1;
		if (tapTempoDivide) mult = c->chaserStepPerTap->floatValue();
		c->chaserSpeed->setValue(val*mult);
	}
	for (Effect* c : computedEffects) {
		double mult = 1;
		if (tapTempoDivide) mult = 1.0/c->beatPerCycle->floatValue();
		c->speed->setValue(val*mult);
	}
	for (Carousel* c : computedCarousels) {
		double mult = 1;
		if (tapTempoDivide) mult = 1.0 / c->beatPerCycle->floatValue();
		c->speed->setValue(val*mult);
	}
	isComputing.exit();
}

void Bundle::setSizeRel(float val, bool size, bool HTP, bool LTP, bool flash)
{
	if (HTP) lastHTP += val;
	if (LTP) lastLTP += val;
	if (size) lastSize += val;
	if (flash) lastFlash += val;
	Brain::getInstance()->virtualFadersNeedUpdate = true;

	computeValues();
	isComputing.enter();
	for (Cuelist* c : computedCuelists) {
		if (HTP) c->HTPLevel->setValue(c->HTPLevel->floatValue()+val);
		if (LTP) c->LTPLevel->setValue(c->LTPLevel->floatValue()+val);
		if (flash) c->flashLevel->setValue(c->flashLevel->floatValue()+val);
	}
	for (Effect* c : computedEffects) {
		if (size) c->sizeValue->setValue(c->sizeValue->floatValue()+val);
		if (flash) c->flashValue->setValue(c->flashValue->floatValue()+val);
	}
	for (Carousel* c : computedCarousels) {
		if (size) c->sizeValue->setValue(c->sizeValue->floatValue()+val);
		if (flash) c->flashValue->setValue(c->flashValue->floatValue()+val);
	}
	for (Mapper* c : computedMappers) {
		if (size) c->sizeValue->setValue(c->sizeValue->floatValue()+val);
	}
	for (Tracker* c : computedTrackers) {
		if (size) c->sizeValue->setValue(c->sizeValue->floatValue()+val);
	}
	isComputing.exit();
}

void Bundle::setSpeedRel(float val, bool tapTempoDivide)
{
	lastSpeed += val;
	Brain::getInstance()->virtualFadersNeedUpdate = true;

	computeValues();
	isComputing.enter();
	for (Cuelist* c : computedCuelists) {
		double mult = 1;
		if (tapTempoDivide) mult = c->chaserStepPerTap->floatValue();
		c->chaserSpeed->setValue(c->chaserSpeed->floatValue() + (val * mult));
	}
	for (Effect* c : computedEffects) {
		double mult = 1;
		if (tapTempoDivide) mult = 1.0 / c->beatPerCycle->floatValue();
		c->speed->setValue(c->speed->floatValue() + (val * mult));
	}
	for (Carousel* c : computedCarousels) {
		double mult = 1;
		if (tapTempoDivide) mult = 1.0 / c->beatPerCycle->floatValue();
		c->speed->setValue(c->speed->floatValue() + (val * mult));
	}
	isComputing.exit();
}

void Bundle::speedMult(float mult)
{
	lastSpeed *= mult;
	Brain::getInstance()->virtualFadersNeedUpdate = true;

	computeValues();
	isComputing.enter();
	for (Cuelist* c : computedCuelists) c->chaserSpeed->setValue(c->chaserSpeed->floatValue() * mult);
	for (Effect* c : computedEffects) c->speed->setValue(c->speed->floatValue() * mult);
	for (Carousel* c : computedCarousels) c->speed->setValue(c->speed->floatValue() * mult);
	isComputing.exit();
}

void Bundle::onControllableFeedbackUpdate(ControllableContainer* cc, Controllable* c) {
	TSLastUpdate = 0;
	computeValues();
}

void Bundle::triggerTriggered(Trigger* t){
}

void Bundle::flash(bool on, bool timing, bool swop)
{
	computeValues();
	isComputing.enter();
	for (Cuelist* c : computedCuelists) c->flash(on, timing, swop);
	for (Effect* c : computedEffects) c->flash(on, swop);
	for (Carousel* c : computedCarousels) c->flash(on, swop);
	isComputing.exit();
}

void Bundle::afterLoadJSONDataInternal() {
}

