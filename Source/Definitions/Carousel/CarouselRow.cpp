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
    wingsInvertSelections = addBoolParameter("Invert wings selection", "Selection are reversed in wings", false);


    addChildControllableContainer(&selection);
    addChildControllableContainer(&paramContainer);

    if (params.isVoid()) {
        selection.addItem();
        paramContainer.addItem();
    }

    updateDisplay();
    if (parentContainer != nullptr && parentContainer->parentContainer != nullptr) {
        parentCarousel = dynamic_cast<Carousel*>(parentContainer->parentContainer.get());
        if (parentCarousel->isOn) {
            parentCarousel->pleaseComputeIfRunning();
        }
    }

};

CarouselRow::~CarouselRow()
{
    if (parentCarousel != nullptr) {
        parentCarousel->isComputing.enter();
        for (auto it = parentCarousel->chanToCarouselRow.begin(); it != parentCarousel->chanToCarouselRow.end(); it.next()) {
            it.getValue()->removeAllInstancesOf(this);
        }
        if (parentCarousel->isOn) {
            parentCarousel->pleaseComputeIfRunning();
        }
        parentCarousel->isComputing.exit();
    }
};

void CarouselRow::computeData() {
    double now = Time::getMillisecondCounterHiRes();
    //LOG("start computing");
    checkParentCarousel();

    HashMap<SubFixture*, double> computedPositions_temp;
    HashMap<SubFixtureChannel*, double> subFixtureChannelOffsets_temp;

    computedPositions.clear();
    subFixtureChannelOffsets.clear();
    if (!enabled->boolValue()) {
        return;
    }

    //LOG("step 1 " << (Time::getMillisecondCounterHiRes() - now));
    selection.computeSelection();
    //LOG("step 2 " << (Time::getMillisecondCounterHiRes() - now));

    if (parentCarousel == nullptr) { return;}
    for (int i = 0; i < selection.computedSelectedSubFixtures.size(); i++) {
        //double deltaPos = 0;
        computedPositions_temp.set(selection.computedSelectedSubFixtures[i], 0);
    }

    //LOG("step 3 " << (Time::getMillisecondCounterHiRes() - now));
    float totalDuration = 0;
    float currentPosition = 0;

    //LOG("step 4 " << (Time::getMillisecondCounterHiRes() - now));
    for (int i = 0; i < paramContainer.items.size(); i++) {
        totalDuration += (float)paramContainer.items[i]->stepDuration->getValue();
    }
    if (totalDuration == 0) { return; }

    //LOG("step 5 " << (Time::getMillisecondCounterHiRes() - now));
    for (int i = 0; i < paramContainer.items.size(); i++) {
        CarouselStep * step = paramContainer.items[i];
        step->relativeDuration = (float)paramContainer.items[i]->stepDuration->getValue() / totalDuration;
        step->relativeStartPosition = currentPosition;
        currentPosition += step->relativeDuration;

        step->computeValues(selection.computedSelectedSubFixtures);

        for (auto it = step->computedValues.begin(); it != paramContainer.items[i]->computedValues.end(); it.next()) {
            if (!parentCarousel->chanToCarouselRow.contains(it.getKey())) {
                parentCarousel->chanToCarouselRow.set(it.getKey(), std::make_shared<Array<CarouselRow*>>());
            }
            parentCarousel->chanToCarouselRow.getReference(it.getKey())->addIfNotAlreadyThere(this);
        }
    }

    //LOG("step 6 " << (Time::getMillisecondCounterHiRes() - now));
    int nWings = wings->getValue();
    int nBlocks = blocks->getValue();
    int nBuddying = buddying->getValue();
    int realTot = ceil(selection.computedSelectedSubFixtures.size() / (float)nBuddying);
    float wingSize = realTot / (float)nWings;
    realTot = ceil(realTot / (float)nWings);
    int roundedWingSize = round(wingSize);
    roundedWingSize = jmax(1, roundedWingSize);
    // int flooredWingSize = floor(wingSize);

    //Array<SubFixtureChannel*> targetChannels;
    HashMap<SubFixtureChannel*, bool> targetChannels;

    for (int i = 1; i <= paramContainer.items.size(); i++) {
        //CarouselStep* currentStep = paramContainer.items[i%paramContainer.items.size()];
        CarouselStep* previousStep = paramContainer.items[i-1];
        for (auto it = previousStep->computedValues.begin(); it != previousStep->computedValues.end(); it.next()) {
            SubFixtureChannel* chan = it.getKey();
            if (!targetChannels.contains(chan)) {targetChannels.set(chan, true);}
        }
    }

    //LOG("step 7 " << (Time::getMillisecondCounterHiRes() - now));
    for (int i = 0; i < paramContainer.items.size(); i++) {
        CarouselStep* currentStep = paramContainer.items[i];
        for (auto it = targetChannels.begin(); it!=targetChannels.end(); it.next()) {
        //for (int ci = 0; ci < targetChannels.size(); ci++) {
            //SubFixtureChannel* chan = targetChannels[ci];
            SubFixtureChannel* chan = it.getKey();
            if (!currentStep->computedValues.contains(chan)) {
                std::shared_ptr<ChannelValue> newVal = std::make_shared<ChannelValue>();
                newVal->values.set(1,-1);
                currentStep->computedValues.set(chan, newVal);
            }
        }
    }


    HashMap<SubFixture*, int> subFixtureToIndex;
    for (int i = 0; i < selection.computedSelectedSubFixtures.size(); i++) {
        SubFixture* sf = selection.computedSelectedSubFixtures[i];
        computedPositions_temp.set(sf, 0);
        subFixtureToIndex.set(sf, i);
    }

    //LOG("step 8 " << (Time::getMillisecondCounterHiRes() - now));
    for (int i = 1; i <= paramContainer.items.size(); i++) {
        CarouselStep* currentStep = paramContainer.items[i % paramContainer.items.size()];
        CarouselStep* previousStep = paramContainer.items[i - 1];
        for (auto it = previousStep->computedValues.begin(); it != previousStep->computedValues.end(); it.next()) {
            SubFixtureChannel* chan = it.getKey();
            std::shared_ptr<ChannelValue> cValue = it.getValue();
            if (!subFixtureChannelOffsets_temp.contains(chan)) {
                if (!subFixtureToIndex.contains(chan->parentSubFixture))
                    continue;

                int chanIndex = subFixtureToIndex.getReference(chan->parentSubFixture);
                int realIndex = chanIndex / nBuddying;

                int nWing = realIndex / wingSize;
                if (nWing % 2 == 1) {
                    realIndex = realIndex % roundedWingSize;
                    realIndex = wingSize - 1 - realIndex;
                }
                realIndex = realIndex * nBlocks;
                realIndex = realIndex % roundedWingSize;

                double offset = realIndex / (double)realTot;

                if (selection.subFixtureToPosition.contains(chan->parentSubFixture)) {
                    offset = selection.subFixtureToPosition.getReference(chan->parentSubFixture);
                    if (offset == 1) { offset = 0.999999999; }
                    offset *= nBlocks;
                    offset = fmod(offset, 1.0f);
                    offset *= nWings;
                    nWing = floor(offset);
                    offset = fmod(offset, 1.0f);
                    if (nWing % 2 == 1) {
                        offset = 1 - offset;
                    }
                }

                offset *= (double)elementsSpread->getValue();
                offset += (double)elementsStart->getValue();
                // //LOG(offset);
                subFixtureChannelOffsets_temp.set(chan, -offset);
            }
            currentStep->computedValues.getReference(chan)->values.set(0,cValue->endValue());
        }

    }

    //LOG("step 9 " << (Time::getMillisecondCounterHiRes() - now));
    if (wingsInvertSelections->boolValue()) {
        double offsetMin = 1;
        double offsetMax = 0;

        for (auto it = subFixtureChannelOffsets_temp.begin(); it != subFixtureChannelOffsets_temp.end(); it.next()) {
            offsetMin = jmin(offsetMin, it.getValue());
            offsetMax = jmax(offsetMax, it.getValue());
        }
        for (auto it = subFixtureChannelOffsets_temp.begin(); it != subFixtureChannelOffsets_temp.end(); it.next()) {
            subFixtureChannelOffsets_temp.set(it.getKey(), jmap(it.getValue(), offsetMin, offsetMax, offsetMax, offsetMin));
        }

    }


    isComputing.enter();
    computedPositions_temp.swapWith(computedPositions);
    subFixtureChannelOffsets_temp.swapWith(subFixtureChannelOffsets);
    isComputing.exit();
}

void CarouselRow::onControllableFeedbackUpdate( ControllableContainer* cc, Controllable* c) {
}

void CarouselRow::updateDisplay() {

    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

void CarouselRow::checkParentCarousel()
{
    if (parentCarousel == nullptr && parentContainer != nullptr && parentContainer->parentContainer != nullptr) {
        parentCarousel = dynamic_cast<Carousel*>(parentContainer->parentContainer.get());
    }
}

