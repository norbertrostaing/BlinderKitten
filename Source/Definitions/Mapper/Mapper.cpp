/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "Mapper.h"
#include "../Command/CommandSelectionManager.h"
#include "../SubFixture/SubFixtureChannel.h"
#include "../../Brain.h"
#include "MapperManager.h"
#include "../CurvePreset/CurvePreset.h"
#include "../ChannelValue.h"
#include "UI/GridView/MapperGridView.h"

Mapper::Mapper(var params) :
	BaseItem(params.getProperty("name", "Mapper")),
	objectType(params.getProperty("type", "Mapper").toString()),
	objectData(params),
	rows("Rows")
{
	saveAndLoadRecursiveData = true;
	nameCanBeChangedByUser = false;

	itemDataType = "Mapper";

	id = addIntParameter("ID", "ID of this Mapper", 1, 1);
	userName = addStringParameter("Name", "Name of this Mapper", "New Mapper");
	layerId = addIntParameter("Layer", "Higher layer, higer priority", 1, 1);
	updateName();

	isMapperOn = addBoolParameter("is ON", "Enable or disable this Mapper",false);
	isMapperOn->isControllableFeedbackOnly;
	isMapperOn->setEnabled(false);
	isMapperOn->isSavable = false;
	isOn = false;

	startBtn = addTrigger("Start", "Start this Mapper");
	stopBtn = addTrigger("Stop", "Stop this Mapper");

	autoStartAndStop = addBoolParameter("Auto Start / Stop", "Start and stop the Mapper when size is modified", true);
	sizeValue = addFloatParameter("Size", "Master of this Mapper", 1, 0, 1);

	rows.selectItemWhenCreated = false;
	addChildControllableContainer(&rows);

	Brain::getInstance()->registerMapper(this, id->getValue());

	if (params.isVoid()) {
		rows.addItem();
	}

}

Mapper::~Mapper()
{
	Brain::getInstance()->unregisterMapper(this);
	Brain::getInstance()->usingCollections.enter();
	Brain::getInstance()->mapperPoolWaiting.removeAllInstancesOf(this);
	Brain::getInstance()->mapperPoolUpdating.removeAllInstancesOf(this);
	Brain::getInstance()->usingCollections.exit();
	isComputing.enter();
	for (auto it = chanToMapperRow.begin(); it != chanToMapperRow.end(); it.next()) {
		SubFixtureChannel* sfc = it.getKey();
		sfc->mapperOutOfStack(this);
		Brain::getInstance()->pleaseUpdate(sfc);
	}
	isComputing.exit();

}

void Mapper::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) {
	pleaseComputeIfRunning();
}


void Mapper::onContainerParameterChangedInternal(Parameter* p) {
	if (p == id) {
		Brain::getInstance()->registerMapper(this, id->getValue(), true);
	}
	if (p == isMapperOn) {
		Brain::getInstance()->virtualButtonsNeedUpdate = true;
		Brain::getInstance()->virtualFaderButtonsNeedUpdate = true;
		Brain::getInstance()->mapperGridNeedRefresh = true;
	}
	if (p == userName || p == id) {
		updateName();
	}
	if (p == sizeValue) {
		if (autoStartAndStop->getValue()) {
			if (isOn && (float)sizeValue->getValue() == 0) {
				stop();
			}
			else if(!isOn && (float)sizeValue->getValue() > 0 && lastSize == 0) {
				start();
			}
		}
		lastSize = p->getValue();
		currentSizeController = nextSizeController;
		nextSizeController = "";
		Brain::getInstance()->virtualFadersNeedUpdate = true;
	}
}

void Mapper::triggerTriggered(Trigger* t) {
	if (t == startBtn) {
		start();
	}
	else if (t == stopBtn) {
		stop();
	}
	else {}
}

void Mapper::start() {
	isOn = true;
	isMapperOn->setValue(true);
	computeData();
}

void Mapper::stop() {
	isOn = false;
	isMapperOn->setValue(false);
	isComputing.enter();
	Array<SubFixtureChannel*> toRemove;
	for (auto it = chanToMapperRow.begin(); it != chanToMapperRow.end(); it.next()) {
		if (it.getKey() != nullptr) {
			toRemove.add(it.getKey());
		}
	}
	isComputing.exit();
	for (SubFixtureChannel* sfc : toRemove) {
		sfc->mapperOutOfStack(this);
	}
}

void Mapper::update(double now) {
	if (computed == false) {
		computeData();
	}
	if (isOn) {
		Brain::getInstance()->pleaseUpdate(this);
	}
}

void Mapper::pleaseComputeIfRunning() {
	if (isOn) {
		computed = false;
		Brain::getInstance()->pleaseUpdate(this);
	}
}

void Mapper::computeData() {
	computed = true;
	isComputing.enter();
	chanToMapperRow.clear();
	for (int i = 0; i < rows.items.size(); i++) {
		rows.items[i]->computeData();
	}
	isComputing.exit();
	if (isOn) {
		Array<SubFixtureChannel*> toUpdate;
		isComputing.enter();
		for (auto it = chanToMapperRow.begin(); it != chanToMapperRow.end(); it.next()) {
			if (it.getKey() != nullptr) {
				it.getKey()->mapperOnTopOfStack(this);
				toUpdate.addIfNotAlreadyThere(it.getKey());
			}
		}
		isComputing.exit();
		for (SubFixtureChannel* sfc : toUpdate) Brain::getInstance()->pleaseUpdate(sfc);

		Brain::getInstance()->pleaseUpdate(this);
	}
}

float Mapper::applyToChannel(SubFixtureChannel* fc, float currentVal, double now) {
	if (!chanToMapperRow.contains(fc)) {return currentVal; }
	if (isOn) {Brain::getInstance()->pleaseUpdate(fc); }
	isComputing.enter();
	float calcValue = currentVal;
	std::shared_ptr<Array<MapperRow*>> activeRows = chanToMapperRow.getReference(fc);
	for (int rId = 0; rId < activeRows->size(); rId++) {
		MapperRow * r = activeRows->getReference(rId);
		
		ChannelType* chanType = dynamic_cast<ChannelType*>(r->followedChannel->targetContainer.get());
		if (chanType != nullptr && fc->parentSubFixture->channelsMap.contains(chanType)) {
			SubFixtureChannel* followedChan = fc->parentSubFixture->channelsMap.getReference(chanType);
			if (followedChan != nullptr) {
				double offset = followedChan->currentValue;
				MapperStep* toApply = nullptr;
				for (int stepId = 0; stepId < r->paramContainer.items.size(); stepId++) {
					MapperStep* step = r->paramContainer.items[stepId];
					if (step->relativeStartPosition <= offset) {
						toApply = step;
					}
				}

				if (toApply == nullptr) {
					isComputing.exit();
					return currentVal;
				}
				std::shared_ptr<ChannelValue> cVal = toApply->computedValues.getReference(fc);
				if (cVal != nullptr) {
					float fadeValue = 1;

					float fadeWidth = 1;
					float pos = (offset - toApply->relativeStartPosition) / toApply->relativeDuration;
					if (pos < fadeWidth) {
						fadeValue = pos / fadeWidth;
						// fadeValue = toApply->curve.getValueAtPosition(fadeValue);
					}

					float start = cVal->startValue;
					float end = cVal->endValue;
					start = start == -1 ? currentVal : start;
					end = end == -1 ? currentVal : end;
					calcValue = jmap(fadeValue, start, end);
				}

			}
		}
	}

	float s = sizeValue->getValue();
	if (fc->isHTP) {
		calcValue *= s;
		currentVal = jmax(calcValue, currentVal);
	}
	else {
		currentVal = jmap(s, currentVal, calcValue);
	}
	isComputing.exit();
	return currentVal;
}


void Mapper::updateName() {
	String n = userName->getValue();
	if (parentContainer != nullptr) {
		dynamic_cast<MapperManager*>(parentContainer.get())->reorderItems();
	}
	setNiceName(String((int)id->getValue()) + " - " + n);
	Brain::getInstance()->reconstructVirtuals = true;
}
