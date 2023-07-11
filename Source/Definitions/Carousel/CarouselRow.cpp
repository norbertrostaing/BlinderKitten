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

    buddying = addIntParameter("Buddying", "make groups of X following subfixtures with the same value", 1, 1);
    blocks = addIntParameter("Blocks", "A Block repeat the effect, applies before wings", 1, 1);
    wings = addIntParameter("Wings", "A wing repeat the effect but inversed", 1, 1);


    addChildControllableContainer(&selection);
    addChildControllableContainer(&paramContainer);

    if (params.isVoid()) {
        selection.addItem();
        paramContainer.addItem();
    }

    updateDisplay();
    if (parentContainer != nullptr && parentContainer->parentContainer != nullptr) {
        ownerCarousel = dynamic_cast<Carousel*>(parentContainer->parentContainer.get());
        if (ownerCarousel->isOn) {
            ownerCarousel->pleaseComputeIfRunning();
        }
    }

};

CarouselRow::~CarouselRow()
{
    if (ownerCarousel != nullptr) {
        ownerCarousel->isComputing.enter();
        for (auto it = ownerCarousel->chanToCarouselRow.begin(); it != ownerCarousel->chanToCarouselRow.end(); it.next()) {
            it.getValue()->removeAllInstancesOf(this);
        }
        if (ownerCarousel->isOn) {
            ownerCarousel->pleaseComputeIfRunning();
        }
        ownerCarousel->isComputing.exit();
    }
};

void CarouselRow::computeData() {
    if (ownerCarousel == nullptr && parentContainer != nullptr && parentContainer->parentContainer != nullptr) {
        ownerCarousel = dynamic_cast<Carousel*>(parentContainer->parentContainer.get());
    }
    isComputing.enter();
    computedPositions.clear();
    subFixtureChannelOffsets.clear();
    if (!enabled->boolValue()) {
        isComputing.exit();
        return;
    }

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

    int nWings = wings->getValue();
    int nBlocks = blocks->getValue();
    int nBuddying = buddying->getValue();
    int realTot = ceil(selection.computedSelectedSubFixtures.size() / (float)nBuddying);
    float wingSize = realTot / (float)nWings;
    realTot = ceil(realTot / (float)nWings);
    int roundedWingSize = round(wingSize);
    // int flooredWingSize = floor(wingSize);

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
                int realIndex = chanIndex / nBuddying;

                int nWing = realIndex / wingSize;
                if (nWing % 2 == 1) {
                    realIndex = realIndex % roundedWingSize;
                    realIndex = wingSize - 1 - realIndex;
                }
                realIndex = realIndex * nBlocks;
                realIndex = realIndex % roundedWingSize;

                double offset = realIndex / (double)realTot;
                offset *= (double)elementsSpread->getValue();
                offset += (double)elementsStart->getValue();
                // LOG(offset);
                subFixtureChannelOffsets.set(chan, -offset);
            }
            currentStep->computedValues.getReference(chan)->startValue = cValue->endValue;
        }

    }

    isComputing.exit();
}

void CarouselRow::onControllableFeedbackUpdate( ControllableContainer* cc, Controllable* c) {
}

void CarouselRow::updateDisplay() {

    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

