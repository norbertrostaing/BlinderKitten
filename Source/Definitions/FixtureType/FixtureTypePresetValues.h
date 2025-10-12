/*
  ==============================================================================

    ChannelType.h
    Created: 7 Nov 2021 7:40:48pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Definitions/Preset/PresetValue.h"

class FixtureTypePresetValues:
    public BaseItem
{
    public:
    FixtureTypePresetValues(var params = var());
    ~FixtureTypePresetValues();

    String objectType;
    var objectData;

    IntParameter* subFixtureId;

    BaseManager<PresetValue> values;
    void onContainerParameterChangedInternal(Parameter* p);

};


