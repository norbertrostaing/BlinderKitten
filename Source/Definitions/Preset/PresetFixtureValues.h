/*
  ==============================================================================

    FixtureParamDefinition.h
    Created: 7 Nov 2021 7:40:48pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "PresetValue.h"

class PresetFixtureValues:
    public BaseItem
{
    public:
    PresetFixtureValues(var params = var());
    ~PresetFixtureValues();

    String objectType;
    var objectData;

    IntParameter* targetFixtureId;
    std::unique_ptr<BaseManager<PresetValue>> values;

    static PresetFixtureValues* create(var params) { return new PresetFixtureValues(params); }
};