/*
  ==============================================================================

    FixtureTypeChannel.cpp
    Created: 8 Nov 2021 7:28:28pm
    Author:  No

  ==============================================================================
*/

#include "MapperRow.h"
#include "../ChannelFamily/ChannelFamilyManager.h"
#include "../ChannelFamily/ChannelType/ChannelType.h"
#include "../SubFixture/SubFixture.h"
#include "../SubFixture/SubFixtureChannel.h"
#include "../ChannelValue.h"
#include "Mapper.h"

MapperRow::MapperRow(var params) :
    BaseItem(params.getProperty("name", "MapperRow")),
    objectType(params.getProperty("type", "MapperRow").toString()),
    objectData(params),
    paramContainer("Steps")
{

    saveAndLoadRecursiveData = true;

    paramContainer.selectItemWhenCreated = false;

    followedChannel = addTargetParameter("Followed Channel", "Channel to follow", ChannelFamilyManager::getInstance());
    followedChannel->targetType = TargetParameter::CONTAINER;
    followedChannel->maxDefaultSearchLevel = 2;

    addChildControllableContainer(&selection);
    addChildControllableContainer(&paramContainer);

    if (params.isVoid()) {
        selection.addItem();
        paramContainer.addItem();
    }

    updateDisplay();
    if (parentContainer != nullptr && parentContainer->parentContainer != nullptr) {
        Mapper* parentMapper = dynamic_cast<Mapper*>(parentContainer->parentContainer.get());
        if (parentMapper->isOn) {
            parentMapper->pleaseComputeIfRunning();
        }
    }

};

MapperRow::~MapperRow()
{
};

void MapperRow::computeData() {
    computedPositions.clear();
    subFixtureChannelOffsets.clear();

    selection.computeSelection();

    Mapper* parentMapper = dynamic_cast<Mapper*>(parentContainer->parentContainer.get());
    if (parentMapper == nullptr) {return;}
    for (int i = 0; i < selection.computedSelectedSubFixtures.size(); i++) {
        double deltaPos = 0;
        computedPositions.set(selection.computedSelectedSubFixtures[i], 0);
    }

    float totalDuration = 0;
    float currentPosition = 0;

    for (int i = 0; i < paramContainer.items.size(); i++) {
        MapperStep * step = paramContainer.items[i];
        
        float stepVal = step->stepValue->getValue();
        if (stepVal >= currentPosition) {
            step->relativeDuration = stepVal - currentPosition;
            step->relativeStartPosition = currentPosition;
            currentPosition = stepVal;

            step->computeValues(selection.computedSelectedSubFixtures);

            for (auto it = step->computedValues.begin(); it != paramContainer.items[i]->computedValues.end(); it.next()) {
                if (!parentMapper->chanToMapperRow.contains(it.getKey())) {
                    parentMapper->chanToMapperRow.set(it.getKey(), new Array<MapperRow*>());
                }
                parentMapper->chanToMapperRow.getReference(it.getKey())->addIfNotAlreadyThere(this);
            }
        }
    }

    float nLimit = selection.computedSelectedSubFixtures.size();
    int sizeWing = nLimit;

    Array<SubFixtureChannel*> targetChannels;

    for (int i = 1; i <= paramContainer.items.size(); i++) {
        MapperStep* currentStep = paramContainer.items[i%paramContainer.items.size()];
        MapperStep* previousStep = paramContainer.items[i-1];
        for (auto it = previousStep->computedValues.begin(); it != previousStep->computedValues.end(); it.next()) {
            SubFixtureChannel* chan = it.getKey();
            if (!targetChannels.contains(chan)) {targetChannels.add(chan);}
        }
    }

    for (int i = 0; i < paramContainer.items.size(); i++) {
        MapperStep* currentStep = paramContainer.items[i];
        for (int ci = 0; ci < targetChannels.size(); ci++) {
            SubFixtureChannel* chan = targetChannels[ci];
            if (!currentStep->computedValues.contains(chan)) {
                ChannelValue* newVal = new ChannelValue();
                newVal->endValue = -1;
                currentStep->computedValues.set(chan, newVal);
            }
        }
    }

    for (int i = 1; i <= paramContainer.items.size(); i++) {
        MapperStep* currentStep = paramContainer.items[i % paramContainer.items.size()];
        MapperStep* previousStep = paramContainer.items[i - 1];
        for (auto it = previousStep->computedValues.begin(); it != previousStep->computedValues.end(); it.next()) {
            SubFixtureChannel* chan = it.getKey();
            ChannelValue* cValue = it.getValue();
            currentStep->computedValues.getReference(chan)->startValue = cValue->endValue;
        }
    }




}

void MapperRow::onControllableFeedbackUpdate( ControllableContainer* cc, Controllable* c) {
}

void MapperRow::updateDisplay() {

    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

