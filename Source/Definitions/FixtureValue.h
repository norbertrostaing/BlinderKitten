/*
  ==============================================================================

    FixtureValue.h
    Created: 17 Nov 2021 4:07:21pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "FixtureParamType/FixtureParamDefinition/FixtureParamDefinition.h"

class ChannelValue {
public:
    float value;
    float delay;
    float fade;
};

class FixtureValue {
public:
    FixtureValue();
    HashMap<FixtureParamDefinition*, ChannelValue*>* values;
    void checkParam(FixtureParamDefinition* chan);
    void setValue(FixtureParamDefinition* chan, float value);
    void setDelay(FixtureParamDefinition* chan, float value);
    void setFade(FixtureParamDefinition* chan, float value);
};

