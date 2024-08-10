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
class EffectRow;

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
    
    IntParameter* buddying;
    IntParameter* blocks;
    IntParameter* wings;
    BoolParameter* wingsInvertSelections;
    BoolParameter* wingsInvertValues;
    BoolParameter* wingsSoloCenterDisable;

    EnumParameter* effectMode;
    TargetParameter* paramType;
    FloatParameter* curveSize;
    BoolParameter* forceBaseValue;
    FloatParameter* baseValue;

    HashMap<SubFixtureChannel*, double> subFixtureChannelOffsets;
    HashMap<SubFixtureChannel*, bool> subFixtureChannelAreWinged;
    HashMap<SubFixtureChannel*, bool> subFixtureChannelAreCentered;

    Effect* parentEffect = nullptr;
    EffectRow* parentEffectRow = nullptr;
    void checkParentEffect();

    void onContainerParameterChangedInternal(Parameter*);
    void updateDisplay();
};