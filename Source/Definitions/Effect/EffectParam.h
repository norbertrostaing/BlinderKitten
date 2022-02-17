/*
  ==============================================================================

    ChannelType.h
    Created: 7 Nov 2021 7:40:48pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "../Command/CommandSelectionManager.h"

class SubFixtureChannel;

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
    
    IntParameter* wings;
    IntParameter* buddying;
    
    EnumParameter* effectMode;
    TargetParameter* paramType;
    FloatParameter* curveSize;
    BoolParameter* forceBaseValue;
    FloatParameter* baseValue;

    HashMap<SubFixtureChannel*, double> subFixtureChannelOffsets;

    void onContainerParameterChangedInternal(Parameter*);
    void updateDisplay();
};