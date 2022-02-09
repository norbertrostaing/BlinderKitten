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
#include "EffectParam.h"

class EffectRow:
    public BaseItem
{
    public:
    EffectRow(var params = var());
    ~EffectRow();

    String objectType;
    var objectData;

    ControllableContainer curveContainer;
    BaseManager<EffectParam> paramContainer;

    EnumParameter* curvePresetOrValue;
    Automation curve;
    FloatParameter* curveOrigin;
    IntParameter* presetId;
    FloatParameter* speed;

    CommandSelectionManager selection;

    HashMap<Fixture*, double> computedPositions;
    void computeData();
};