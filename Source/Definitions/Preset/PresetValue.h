/*
  ==============================================================================

    FixtureParamDefinition.h
    Created: 7 Nov 2021 7:40:48pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h";

class PresetValue:
    public BaseItem
{
    public:
    PresetValue(var params = var());
    ~PresetValue();

    String objectType;
    var objectData;

    TargetParameter* param;
    FloatParameter* paramValue;

    static PresetValue* create(var params) { return new PresetValue(params); }
};