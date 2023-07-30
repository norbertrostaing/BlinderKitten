/*
  ==============================================================================

    FixtureTypeChannel.cpp
    Created: 8 Nov 2021 7:28:28pm
    Author:  No

  ==============================================================================
*/

#include "MapperStep.h"
#include "../ChannelFamily/ChannelFamilyManager.h"
#include "Mapper.h"
#include "../ChannelValue.h"
#include "../SubFixture/SubFixtureChannel.h"
#include "../Preset/Preset.h"
#include "../../Brain.h"

MapperStep::MapperStep(var params) :
    BaseItem(params.getProperty("name", "MapperStep")),
    objectType(params.getProperty("type", "MapperStep").toString()),
    objectData(params)
{
    saveAndLoadRecursiveData = true;

	stepValue = addFloatParameter("Step value", "Pause time on the value, unit is relative to other", 0, 0, 1);
	//stepFade = addFloatParameter("Step Fade", "Fade size", 0, 0, 1);
	

    //curve.saveAndLoadRecursiveData = true;
    //curve.editorIsCollapsed = true;
    //curve.allowKeysOutside = false;
    //curve.isSelectable = false;
    //curve.length->setValue(1);
    //curve.addKey(0, 0, false);
    //curve.items[0]->easingType->setValueWithData(Easing::LINEAR);
    //curve.addKey(1, 1, false);
    //curve.selectItemWhenCreated = false;
    //curve.editorCanBeCollapsed = true;
    //curve.editorIsCollapsed = false;

    //addChildControllableContainer(&curve);
    addChildControllableContainer(&values);

	if (params.isVoid()) {
		values.addItem();
	}

    updateDisplay();
};

MapperStep::~MapperStep()
{
};


void MapperStep::onContainerParameterChangedInternal(Parameter* c) {
}

void MapperStep::updateDisplay() {
    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}


void MapperStep::computeValues(Array<SubFixture*> SubFixtures) {
	computedValues.clear();
	Array<CommandValue*> commandValues = values.getItemsWithType<CommandValue>();

	Automation* fadeCurve;
	Automation* fadeRepartCurve;
	Automation* delayRepartCurve;

	for (int commandIndex = 0; commandIndex < commandValues.size(); commandIndex++) {
		CommandValue* cv = commandValues[commandIndex];
		bool symValues = cv->symmetry->getValue();
		Preset* pFrom = nullptr;
		Preset* pTo = nullptr;
		if (cv->presetOrValue->getValue() == "preset") {
			pFrom = Brain::getInstance()->getPresetById(cv->presetIdFrom->getValue());
			pTo = Brain::getInstance()->getPresetById(cv->presetIdTo->getValue());
			if (pFrom != nullptr) {
				pFrom->computeValues();
			}
			if (pTo != nullptr) {
				pTo->computeValues();
			}
		}

		ChannelType* rawChan = dynamic_cast<ChannelType*>(cv->channelType->targetContainer.get());
		for (int indexFixt = 0; indexFixt < SubFixtures.size(); indexFixt++) {

			std::shared_ptr < HashMap<ChannelType*, float>> valuesFrom = std::make_shared<HashMap<ChannelType*, float>>();
			std::shared_ptr < HashMap<ChannelType*, float>> valuesTo = std::make_shared<HashMap<ChannelType*, float>>();
			String test = cv->presetOrValue->getValue();
			if (cv->presetOrValue->getValue() == "preset") {

				std::shared_ptr < HashMap<ChannelType*, float>> tempValuesFrom = pFrom != nullptr ? pFrom->getSubFixtureValues(SubFixtures[indexFixt]) : nullptr;
				std::shared_ptr < HashMap<ChannelType*, float>> tempValuesTo = pTo != nullptr ? pTo->getSubFixtureValues(SubFixtures[indexFixt]) : nullptr;
				if (tempValuesFrom != nullptr) {
					for (auto it = tempValuesFrom->begin(); it != tempValuesFrom->end(); it.next()) {
						valuesFrom->set(it.getKey(), it.getValue());
					}
					//delete tempValuesFrom;
				}
				if (tempValuesTo != nullptr) {
					for (auto it = tempValuesTo->begin(); it != tempValuesTo->end(); it.next()) {
						valuesTo->set(it.getKey(), it.getValue());
					}
					//delete tempValuesTo;
				}

			}

			if (cv->presetOrValue->getValue() == "value") {
				valuesFrom->set(rawChan, cv->valueFrom->getValue());
				valuesTo->set(rawChan, cv->valueTo->getValue());
			}

			for (auto it = valuesFrom->begin(); it != valuesFrom->end(); it.next()) {
				SubFixtureChannel* fchan = SubFixtures[indexFixt]->channelsMap.contains(it.getKey()) ? SubFixtures[indexFixt]->channelsMap.getReference(it.getKey()) : nullptr;

				float valueFrom = it.getValue();
				float valueTo = valueFrom;
				if (valuesTo->contains(it.getKey())) {
					valueTo = valuesTo->getReference(it.getKey());
				}

				if (fchan != nullptr) {
					if (!computedValues.contains(fchan)) {
						computedValues.set(fchan, std::make_shared<ChannelValue>());
					}
					std::shared_ptr<ChannelValue> finalValue = computedValues.getReference(fchan);
					float val = valueFrom;
					if (cv->thru->getValue() && SubFixtures.size() > 1) {
						float position = float(indexFixt) / float(SubFixtures.size() - 1);
						if (symValues) { position = Brain::symPosition(indexFixt, SubFixtures.size()); }
						val = jmap(position, val, valueTo);
					}
					finalValue->endValue = val;
				}
			}

			if (valuesFrom != nullptr) {
				//delete valuesFrom;
			}
			if (valuesTo != nullptr) {
				//delete valuesTo;
			}
		}
	}
}

