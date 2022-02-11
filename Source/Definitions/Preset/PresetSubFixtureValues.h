/*
  ==============================================================================

    ChannelType.h
    Created: 7 Nov 2021 7:40:48pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "PresetValue.h"

class PresetSubFixtureValues:
    public BaseItem
{
    public:
    PresetSubFixtureValues(var params = var());
    ~PresetSubFixtureValues();

    String objectType;
    var objectData;

    IntParameter* targetFixtureId;
    IntParameter* targetSubFixtureId;
    std::unique_ptr<BaseManager<PresetValue>> values;

    static PresetSubFixtureValues* create(var params) { return new PresetSubFixtureValues(params); }
};