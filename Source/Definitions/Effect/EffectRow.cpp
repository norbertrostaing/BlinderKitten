/*
  ==============================================================================

    FixtureTypeChannel.cpp
    Created: 8 Nov 2021 7:28:28pm
    Author:  No

  ==============================================================================
*/

#include "EffectRow.h"
#include "../ChannelFamily/ChannelFamilyManager.h"
#include "../ChannelFamily/ChannelType/ChannelType.h"
#include "../SubFixture/SubFixture.h"
#include "../SubFixture/SubFixtureChannel.h"
#include "Effect.h"

EffectRow::EffectRow(var params) :
    BaseItem(params.getProperty("name", "EffectRow")),
    objectType(params.getProperty("type", "EffectRow").toString()),
    objectData(params),
    curveContainer("Curve"),
    paramContainer("Parameters"),
    curve("Curve")
{

    saveAndLoadRecursiveData = true;
    speed = curveContainer.addFloatParameter("Speed", "Effect of this Curve, relative to the effect speed", 1, 0);
    direction = curveContainer.addEnumParameter("Direction", "");
    direction->addOption("Forward", "forward");
    direction->addOption("Backward", "backward");
    direction->addOption("Bounce", "bounce");
    curvePresetOrValue = curveContainer.addEnumParameter("Curve Type", "Use a preset curve or draw it here ?");
    curvePresetOrValue->addOption("Drawed", "drawed");
    curvePresetOrValue->addOption("Preset", "preset");
    curvePresetOrValue->addOption("Chaser", "chaser");

    paramContainer.selectItemWhenCreated = false;

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
    curveContainer.addChildControllableContainer(&curve);

    presetId = curveContainer.addIntParameter("Curve preset ID", "ID of the curve preset you want to use", 1, 1);
    chaserFade = curveContainer.addFloatParameter("Chaser Fade","",0,0,1);
    chaserBuddying = curveContainer.addIntParameter("Chaser Buddying", "Multiplies the chaser on time duration",1,1);

    curveOrigin = curveContainer.addFloatParameter("Curve Origin", "Y Value of the curve to use as origin", 0, 0, 1);

    curveContainer.saveAndLoadRecursiveData = true;
    paramContainer.saveAndLoadRecursiveData = true;

    addChildControllableContainer(&curveContainer);
    addChildControllableContainer(&selection);
    addChildControllableContainer(&paramContainer);

    if (params.isVoid()) {
        selection.addItem();
        paramContainer.addItem();
    }

    updateDisplay();
    if (parentContainer != nullptr && parentContainer->parentContainer != nullptr) {
        Effect* parentEffect = dynamic_cast<Effect*>(parentContainer->parentContainer.get());
        if (parentEffect->isOn) {
            parentEffect->pleaseComputeIfRunning();
        }

    }

};

EffectRow::~EffectRow()
{
};

void EffectRow::computeData() {
    isComputing.enter();
    computedPositions.clear();
    selection.computeSelection();
    if (!enabled->boolValue()) {
        isComputing.exit();
        return;
    }
    if (parentContainer == nullptr || parentContainer->parentContainer == nullptr ) {
        isComputing.exit();
        return;
    }
    Effect* parentEffect = dynamic_cast<Effect*>(parentContainer->parentContainer.get());
    if (parentEffect == nullptr) {return;}
    for (int i = 0; i < selection.computedSelectedSubFixtures.size(); i++) {
        //double deltaPos = 0;
        computedPositions.set(selection.computedSelectedSubFixtures[i], 0);
    }
    for (int i = 0; i < paramContainer.items.size(); i++) {
        EffectParam* p = paramContainer.items[i];
        if (p->enabled->boolValue()) {
            p->subFixtureChannelOffsets.clear();
            p->subFixtureChannelAreWinged.clear();
            p->subFixtureChannelAreCentered.clear();
            ChannelType* chanType = dynamic_cast<ChannelType*>(p->paramType->targetContainer.get());
            if (chanType != nullptr) {
                Array<SubFixtureChannel*> chans;
                for (int fIndex = 0; fIndex < selection.computedSelectedSubFixtures.size(); fIndex++) {
                    SubFixture* f = selection.computedSelectedSubFixtures[fIndex];
                    if (f->channelsMap.contains(chanType)) {
                        chans.add(f->channelsMap.getReference(chanType));
                    }
                }
            
                int nWings = p->wings->getValue();
                int nBlocks = p->blocks->getValue();
                int nBuddying = p->buddying->getValue();
                int realTot = ceil(chans.size()/(float)nBuddying);
                float wingSize = realTot / (float)nWings;
                realTot = ceil(realTot / (float)nWings);
                int roundedWingSize = round(wingSize);
                int flooredWingSize = floor(wingSize);


                for (int chanIndex = 0; chanIndex < chans.size(); chanIndex++) {
                    int realIndex = chanIndex/nBuddying;

                    int nWing = realIndex/wingSize;
                    bool isWinged = false;
                    bool isCentered = false;
                    if (nWing % 2 == 1) {
                        isWinged = true;
                        realIndex = realIndex % roundedWingSize;
                        realIndex = wingSize - 1 - realIndex;
                    }
                    else if (realIndex == flooredWingSize && wingSize > realIndex) {
                        isCentered = true;
                    }
                    realIndex = realIndex*nBlocks;
                    realIndex = realIndex % roundedWingSize;

                    double offset = realIndex / (double)realTot;
                    offset *= (double)p->elementsSpread->getValue();
                    offset += (double)p->elementsStart->getValue();
                    if (p->wingsInvertSelections->boolValue()) {
                        offset = 1 - offset;
                    }
                    p->subFixtureChannelOffsets.set(chans[chanIndex], -offset);
                    p->subFixtureChannelAreWinged.set(chans[chanIndex], isWinged);
                    p->subFixtureChannelAreCentered.set(chans[chanIndex], isCentered);
                    if (!parentEffect->chanToFxParam.contains(chans[chanIndex])) {
                        parentEffect->chanToFxParam.set(chans[chanIndex], std::make_shared<Array<EffectParam*>>());
                    }
                    parentEffect->chanToFxParam.getReference(chans[chanIndex])->add(p);
                }
            }
        }
    }
    isComputing.exit();
}

void EffectRow::onControllableFeedbackUpdate( ControllableContainer* cc, Controllable* c) {
    if (c == curvePresetOrValue) {
        updateDisplay();
    }
}

void EffectRow::updateDisplay() {
    //bool curveIsPreset = curvePresetOrValue->getValue() == "preset";
    String mode = curvePresetOrValue->getValue().toString();
    presetId->hideInEditor = !(mode == "preset");
    curve.hideInEditor = !(mode == "drawed");
    chaserFade->hideInEditor = !(mode == "chaser");
    chaserBuddying->hideInEditor = !(mode == "chaser");
    curveOrigin->hideInEditor = mode == "chaser";

    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

