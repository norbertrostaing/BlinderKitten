/*
  ==============================================================================

    FixtureTypeChannel.cpp
    Created: 8 Nov 2021 7:28:28pm
    Author:  No

  ==============================================================================
*/

#include "CarouselStep.h"
#include "../ChannelFamily/ChannelFamilyManager.h"
#include "Carousel.h"
#include "../ChannelValue.h"
#include "../SubFixture/SubFixtureChannel.h"
#include "../Preset/Preset.h"
#include "../../Brain.h"

CarouselStep::CarouselStep(var params) :
    BaseItem(params.getProperty("name", "CarouselStep")),
    objectType(params.getProperty("type", "CarouselStep").toString()),
    objectData(params)
{
    saveAndLoadRecursiveData = true;

    stepDuration = addFloatParameter("Step duration", "Pause time on the value, unit is relative to other", 1, 0);
    fadeRatio = addFloatParameter("Fade", "Transition fade", 1, 0, 1);

    curve.saveAndLoadRecursiveData = true;
    curve.editorIsCollapsed = true;
    curve.allowKeysOutside = false;
    curve.isSelectable = false;
    curve.length->setValue(1);
    curve.addKey(0, 0, false);
    curve.items[0]->easingType->setValueWithData(Easing::LINEAR);
    curve.addKey(1, 1, false);
    curve.selectItemWhenCreated = false;
    curve.editorCanBeCollapsed = true;
    curve.editorIsCollapsed = false;

    addChildControllableContainer(&curve);
    addChildControllableContainer(&values);

	if (params.isVoid()) {
		values.addItem();
	}

    updateDisplay();
};

CarouselStep::~CarouselStep()
{
	if (parentCarousel != nullptr) {
		parentCarousel->computeData();
	}
};


void CarouselStep::checkParentCarousel()
{
	if (parentCarousel == nullptr && 
		parentContainer != nullptr && 
		parentContainer->parentContainer != nullptr &&
		parentContainer->parentContainer->parentContainer != nullptr &&
		parentContainer->parentContainer->parentContainer->parentContainer != nullptr
		) {
		parentCarousel = dynamic_cast<Carousel*>(parentContainer->parentContainer.get());
	}
}

void CarouselStep::onContainerParameterChangedInternal(Parameter* c) {
}

void CarouselStep::updateDisplay() {
    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}


void CarouselStep::computeValues(Array<SubFixture*> SubFixtures) {
	isComputing.enter();
	computedValues.clear();
	if (!enabled->boolValue()) {
		isComputing.exit();
		return;
	}
	Array<CommandValue*> commandValues = values.getItemsWithType<CommandValue>();

	for (int commandIndex = 0; commandIndex < commandValues.size(); commandIndex++) {
		CommandValue* cv = commandValues[commandIndex];
		bool symValues = cv->symmetry->getValue();
		Preset* pFrom = nullptr;
		Preset* pTo = nullptr;
		if (cv->presetOrValue->getValue() == "preset") {
			pFrom = Brain::getInstance()->getPresetById(cv->presetIdFrom->getValue(), true);
			pTo = Brain::getInstance()->getPresetById(cv->presetIdTo->getValue(), true);
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
					//tempValuesFrom->~HashMap();
				}
				if (tempValuesTo != nullptr) {
					for (auto it = tempValuesTo->begin(); it != tempValuesTo->end(); it.next()) {
						valuesTo->set(it.getKey(), it.getValue());
					}
					//tempValuesTo->~HashMap();
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
					finalValue->values.set(1, val);
					finalValue->htpOverride = cv->HTPOverride->boolValue();
				}
			}

			if (valuesFrom != nullptr) {
				//valuesFrom->~HashMap();
			}
			if (valuesTo != nullptr) {
				//valuesTo->~HashMap();
			}
		}
	}
	isComputing.exit();
}

