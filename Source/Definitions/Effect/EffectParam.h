/*
  ==============================================================================

    FixtureParamDefinition.h
    Created: 7 Nov 2021 7:40:48pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "../Command/CommandSelectionManager.h"

class FixtureChannel;

class EffectParam:
    public BaseItem
{
    public:
    EffectParam(var params = var());
    ~EffectParam();

    String objectType;
    var objectData;

    FloatParameter* elementsStart;
    FloatParameter* elementsSpread;
    EnumParameter* effectMode;
    TargetParameter* paramType;
    FloatParameter* curveSize;
    BoolParameter* forceBaseValue;
    FloatParameter* baseValue;

    HashMap<FixtureChannel*, double> fixtureChannelOffsets;

    void onContainerParameterChangedInternal(Parameter*);
    void updateDisplay();
};