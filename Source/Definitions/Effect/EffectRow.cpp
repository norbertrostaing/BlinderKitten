/*
  ==============================================================================

    DeviceTypeChannel.cpp
    Created: 8 Nov 2021 7:28:28pm
    Author:  No

  ==============================================================================
*/

#include "EffectRow.h"
#include "../FixtureParamType/FixtureParamTypeManager.h"
#include "../FixtureParamType/FixtureParamDefinition/FixtureParamDefinition.h"
#include "../Fixture/Fixture.h"
#include "../Fixture/FixtureChannel.h"
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
    curvePresetOrValue = curveContainer.addEnumParameter("Curve Type", "Use a preset curve or draw it here ?");
    curvePresetOrValue->addOption("Drawed", "drawed");
    curvePresetOrValue->addOption("Preset", "preset");

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

    presetId = curveContainer.addIntParameter("Curve preset ID", "ID of the curve preset you want to use", 0, 0);
    curveOrigin = curveContainer.addFloatParameter("Curve Origin", "Y Value of the curve to use as origin", 0, 0, 1);

    curveContainer.saveAndLoadRecursiveData = true;
    paramContainer.saveAndLoadRecursiveData = true;

    addChildControllableContainer(&curveContainer);
    addChildControllableContainer(&selection);
    addChildControllableContainer(&paramContainer);
    

};

EffectRow::~EffectRow()
{
};


void EffectRow::computeData() {
    computedPositions.clear();
    selection.computeSelection();
    String test = parentContainer->parentContainer->niceName;
    Effect* parentEffect = dynamic_cast<Effect*>(parentContainer->parentContainer.get());
    if (parentEffect == nullptr) {return;}
    for (int i = 0; i < selection.computedSelectedFixtures.size(); i++) {
        double deltaPos = 0;
        computedPositions.set(selection.computedSelectedFixtures[i], 0);
    }
    for (int i = 0; i < paramContainer.items.size(); i++) {
        EffectParam* p = paramContainer.items[i];
        p -> fixtureChannelOffsets.clear();
        FixtureParamDefinition* chanType = dynamic_cast<FixtureParamDefinition*>(p->paramType->targetContainer.get());
        if (chanType != nullptr) {
            Array<FixtureChannel*> chans;
            for (int fIndex = 0; fIndex < selection.computedSelectedFixtures.size(); fIndex++) {
                Fixture* f = selection.computedSelectedFixtures[fIndex];
                FixtureChannel* c = f->channelsMap.getReference(chanType);
                if (c != nullptr) {
                    chans.add(c);
                }
            }
            for (int chanIndex = 0; chanIndex < chans.size(); chanIndex++) {
                double offset = chanIndex / (double)chans.size();
                offset *= (double)p->elementsSpread->getValue();
                offset += (double)p->elementsStart->getValue();
                p->fixtureChannelOffsets.set(chans[chanIndex], -offset);
                if (!parentEffect->chanToFxParam.contains(chans[chanIndex])) {
                    parentEffect->chanToFxParam.set(chans[chanIndex], new Array<EffectParam*>());
                }
                parentEffect->chanToFxParam.getReference(chans[chanIndex])->add(p);
            }
        }
    }
}

