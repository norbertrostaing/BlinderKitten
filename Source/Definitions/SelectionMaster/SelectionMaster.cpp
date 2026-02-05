/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "SelectionMaster.h"
#include "../Command/CommandSelectionManager.h"
#include "../SubFixture/SubFixtureChannel.h"
#include "../../Brain.h"
#include "SelectionMasterManager.h"
#include "../CurvePreset/CurvePreset.h"
#include "../ChannelValue.h"
#include "Definitions/ChannelFamily/ChannelFamilyManager.h"
#include "BKEngine.h"

SelectionMaster::SelectionMaster(var params) :
	BaseItem(params.getProperty("name", "SelectionMaster")),
	objectType(params.getProperty("type", "SelectionMaster").toString()),
	objectData(params),
	selection()
{
	saveAndLoadRecursiveData = true;
	nameCanBeChangedByUser = false;

	itemDataType = "SelectionMaster";

	id = addIntParameter("ID", "ID of this SelectionMaster", 1, 1);
	userName = addStringParameter("Name", "Name of this SelectionMaster", "New SelectionMaster");
	layerId = addIntParameter("Layer", "Higher layer, higer priority", 1, 1);
	updateName();


	channelType = addTargetParameter("Followed Channel", "Channel to follow", ChannelFamilyManager::getInstance());
	channelType->targetType = TargetParameter::CONTAINER;
	channelType->maxDefaultSearchLevel = 2;
	channelType->typesFilter.add("ChannelType");

	masterType = addEnumParameter("Master mode", "type of master you want");
	masterType->addOption("Scale", SCALE)->addOption("Inhibit", INHIBIT)->addOption("Offset", OFFSET);

	isSelectionMasterOn = addBoolParameter("is ON", "Enable or disable this SelectionMaster",false);
    // is this supposed to be setControllableFeedbackOnly ?
	// isSelectionMasterOn->isControllableFeedbackOnly;
	isSelectionMasterOn->setEnabled(false);
	isSelectionMasterOn->isSavable = false;
	isOn = false;

	startBtn = addTrigger("Start", "Start this SelectionMaster");
	stopBtn = addTrigger("Stop", "Stop this SelectionMaster");

	autoStartAndStop = addBoolParameter("Auto Start / Stop", "Start and stop the SelectionMaster when size is modified", true);
	sizeValue = addFloatParameter("Size", "Master of this SelectionMaster", 1);

	addChildControllableContainer(&selection);

	Brain::getInstance()->registerSelectionMaster(this, id->getValue());
	updateSelection();
}

SelectionMaster::~SelectionMaster()
{
	Brain::getInstance()->unregisterSelectionMaster(this);
	Brain::getInstance()->usingCollections.enter();
	Brain::getInstance()->selectionMasterPoolWaiting.removeAllInstancesOf(this);
	Brain::getInstance()->selectionMasterPoolUpdating.removeAllInstancesOf(this);
	Brain::getInstance()->usingCollections.exit();
	isComputing.enter();
	for (SubFixtureChannel* sfc : channels) {
		sfc->selectionMasterOutOfStack(this);
		Brain::getInstance()->pleaseUpdate(sfc);
	}
	isComputing.exit();
}

void SelectionMaster::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) {
	updateSelection();
}


void SelectionMaster::onContainerParameterChangedInternal(Parameter* p) {
	if (p == id) {
		Brain::getInstance()->registerSelectionMaster(this, id->getValue(), true);
	}
	if (p == isSelectionMasterOn) {
		Brain::getInstance()->virtualButtonsNeedUpdate = true;
		Brain::getInstance()->virtualFaderButtonsNeedUpdate = true;
	}
	if (p == userName || p == id) {
		updateName();
	}
	if (p == sizeValue) {
		if (autoStartAndStop->boolValue()) {
			float compareVal = 0;
			float newVal = sizeValue->floatValue();
			switch (masterType->getValueDataAsEnum<MasterType>()) {
				case INHIBIT: compareVal = 1; break;
				case SCALE: compareVal = 1; break;
				case OFFSET: compareVal = 0; break;
			}
			if (lastSize == compareVal && newVal != compareVal) {start(); }
			else if (lastSize != compareVal && newVal == compareVal) { stop(); }
			else if (lastSize < compareVal && newVal > compareVal) { start(); }
			else if (lastSize > compareVal && newVal < compareVal) { start(); }
		}
		lastSize = p->getValue();
		currentSizeController = nextSizeController;
		nextSizeController = "";
		Brain::getInstance()->virtualFadersNeedUpdate = true;
		updateAllSubFixtureChannels();
	}
	if (p == masterType) {
		updateAllSubFixtureChannels();
	}
	if (p == channelType) {
		updateSelection();
	}
}

void SelectionMaster::triggerTriggered(Trigger* t) {
	if (t == startBtn) {
		start();
	}
	else if (t == stopBtn) {
		stop();
	}
	else {}
}

void SelectionMaster::start() {
	isOn = true;
	isSelectionMasterOn->setValue(true);
	updateSelection();
}

void SelectionMaster::stop() {
	isOn = false;
	isSelectionMasterOn->setValue(false);
	for (SubFixtureChannel* sfc : channels) {
		if (sfc != nullptr) {
			Brain::getInstance()->pleaseUpdate(sfc);
		}
	}
}

void SelectionMaster::update(double now) {
}

void SelectionMaster::computeData() {
	isComputing.enter();
	channels.clear();
	ChannelType *ct = dynamic_cast<ChannelType*>(channelType->targetContainer.get());
	if (ct != nullptr) {
		selection.computeSelection();
		for (SubFixture* sf : selection.computedSelectedSubFixtures) {
			if (sf->channelsMap.contains(ct)) {
				channels.add(sf->channelsMap.getReference(ct));
			}
		}
	}
	isComputing.exit();
}

void SelectionMaster::updateSelection(bool restart)
{
	Array<SubFixtureChannel*> oldChannels;
	oldChannels.addArray(channels);
	computeData();
	for (SubFixtureChannel* sfc : channels) {
		oldChannels.removeAllInstancesOf(sfc);
	}
	for (SubFixtureChannel* sfc : oldChannels) {
		sfc->selectionMasterOutOfStack(this);
		Brain::getInstance()->pleaseUpdate(sfc);
	}
	for (SubFixtureChannel* sfc : channels) {
		if (!sfc->selectionMasterStack.contains(this) || restart) {
			sfc->selectionMasterOnTopOfStack(this);
			Brain::getInstance()->pleaseUpdate(sfc);
		}
	}
}

float SelectionMaster::applyToChannel(SubFixtureChannel* fc, float currentVal, double now) {
	if (!channels.contains(fc)) {return currentVal; }
	if (computing) { return currentVal; }

	MasterType type = masterType->getValueDataAsEnum<MasterType>();
	float size = sizeValue->floatValue();
	switch (type) {
		case SCALE: currentVal *= size; break;
		case INHIBIT: currentVal = jmin(size, currentVal); break;
		case OFFSET: currentVal += size; break;
	}
	currentVal = jlimit(0.f,1.f,currentVal);
	return currentVal;
}

void SelectionMaster::updateAllSubFixtureChannels()
{
	for (SubFixtureChannel* sfc : channels) {
		Brain::getInstance()->pleaseUpdate(sfc);
	}
}


void SelectionMaster::updateName() {
	String n = userName->getValue();
	if (parentContainer != nullptr) {
		dynamic_cast<SelectionMasterManager*>(parentContainer.get())->reorderItems();
	}
	setNiceName(String((int)id->getValue()) + " - " + n);
}
