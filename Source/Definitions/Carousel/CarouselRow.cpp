/*
  ==============================================================================

    FixtureTypeChannel.cpp
    Created: 8 Nov 2021 7:28:28pm
    Author:  No

  ==============================================================================
*/

#include "CarouselRow.h"
#include "../ChannelFamily/ChannelFamilyManager.h"
#include "../ChannelFamily/ChannelType/ChannelType.h"
#include "../SubFixture/SubFixture.h"
#include "../SubFixture/SubFixtureChannel.h"
#include "../ChannelValue.h"
#include "Carousel.h"

CarouselRow::CarouselRow(var params) :
    BaseItem(params.getProperty("name", "CarouselRow")),
    objectType(params.getProperty("type", "CarouselRow").toString()),
    objectData(params),
    paramContainer("Steps")
{

    saveAndLoadRecursiveData = true;

    paramContainer.selectItemWhenCreated = false;

    elementsStart = addFloatParameter("First Element position", "Position of the first element, relative to the curve", 0, 0, 1);
    elementsSpread = addFloatParameter("Elements Spread", "Size of distribution", 1, 0);

    wings = addIntParameter("Wings", "A wing repeat the effect but inversed", 1, 1);
    buddying = addIntParameter("Buddying", "make groups of X following subfixtures with the same value", 1, 1);


    addChildControllableContainer(&selection);
    addChildControllableContainer(&paramContainer);

    if (params.isVoid()) {
        selection.addItem();
        paramContainer.addItem();
    }

    updateDisplay();
    if (parentContainer != nullptr && parentContainer->parentContainer != nullptr) {
        Carousel* parentCarousel = dynamic_cast<Carousel*>(parentContainer->parentContainer.get());
        if (parentCarousel->isOn) {
            parentCarousel->pleaseComputeIfRunning();
        }
    }

};

CarouselRow::~CarouselRow()
{
    if (parentContainer != nullptr && parentContainer->parentContainer != nullptr) {
        LOG("");
        Carousel* parentCarousel = dynamic_cast<Carousel*>(parentContainer->parentContainer.get());
        for (auto it = parentCarousel->chanToCarouselRow.begin(); it != parentCarousel->chanToCarouselRow.end(); it.next()) {
            it.getValue()->removeAllInstancesOf(this);
        }
        if (parentCarousel->isOn) {
            parentCarousel->pleaseComputeIfRunning();
        }
    }
};

void CarouselRow::computeData() {
    computedPositions.clear();
    subFixtureChannelOffsets.clear();

    selection.computeSelection();

    Carousel* parentCarousel = dynamic_cast<Carousel*>(parentContainer->parentContainer.get());
    if (parentCarousel == nullptr) {return;}
    for (int i = 0; i < selection.computedSelectedSubFixtures.size(); i++) {
        //double deltaPos = 0;
        computedPositions.set(selection.computedSelectedSubFixtures[i], 0);
    }

    float totalDuration = 0;
    float currentPosition = 0;

    for (int i = 0; i < paramContainer.items.size(); i++) {
        totalDuration += (float)paramContainer.items[i]->stepDuration->getValue();
    }
    if (totalDuration == 0) {return; }

    for (int i = 0; i < paramContainer.items.size(); i++) {
        CarouselStep * step = paramContainer.items[i];
        step->relativeDuration = (float)paramContainer.items[i]->stepDuration->getValue() / totalDuration;
        step->relativeStartPosition = currentPosition;
        currentPosition += step->relativeDuration;

        step->computeValues(selection.computedSelectedSubFixtures);

        for (auto it = step->computedValues.begin(); it != paramContainer.items[i]->computedValues.end(); it.next()) {
            if (!parentCarousel->chanToCarouselRow.contains(it.getKey())) {
                parentCarousel->chanToCarouselRow.set(it.getKey(), new Array<CarouselRow*>());
            }
            parentCarousel->chanToCarouselRow.getReference(it.getKey())->addIfNotAlreadyThere(this);
        }
    }

    int totWings = wings->getValue();
    int nBuddying = buddying->getValue();
    float nLimit = selection.computedSelectedSubFixtures.size();
    int sizeWing = nLimit / totWings;

    Array<SubFixtureChannel*> targetChannels;

    for (int i = 1; i <= paramContainer.items.size(); i++) {
        //CarouselStep* currentStep = paramContainer.items[i%paramContainer.items.size()];
        CarouselStep* previousStep = paramContainer.items[i-1];
        for (auto it = previousStep->computedValues.begin(); it != previousStep->computedValues.end(); it.next()) {
            SubFixtureChannel* chan = it.getKey();
            if (!targetChannels.contains(chan)) {targetChannels.add(chan);}
        }
    }

    for (int i = 0; i < paramContainer.items.size(); i++) {
        CarouselStep* currentStep = paramContainer.items[i];
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
        CarouselStep* currentStep = paramContainer.items[i % paramContainer.items.size()];
        CarouselStep* previousStep = paramContainer.items[i - 1];
        for (auto it = previousStep->computedValues.begin(); it != previousStep->computedValues.end(); it.next()) {
            SubFixtureChannel* chan = it.getKey();
            ChannelValue* cValue = it.getValue();
            if (!subFixtureChannelOffsets.contains(chan)) {

                int chanIndex = selection.computedSelectedSubFixtures.indexOf(chan->parentSubFixture);
                int nWing = chanIndex / sizeWing;
                double offset = (chanIndex - (chanIndex % nBuddying)) / nLimit;
                if (nWing % 2 == 1) {
                    offset = -offset;
                }
                offset *= (double)elementsSpread->getValue();
                offset += (double)elementsStart->getValue();

                // LOG(offset);
                subFixtureChannelOffsets.set(chan, offset);
            }
            currentStep->computedValues.getReference(chan)->startValue = cValue->endValue;
        }

    }




}

void CarouselRow::onControllableFeedbackUpdate( ControllableContainer* cc, Controllable* c) {
}

void CarouselRow::updateDisplay() {

    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

