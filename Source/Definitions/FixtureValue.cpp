/*
  ==============================================================================

    FixtureValue.cpp
    Created: 17 Nov 2021 4:07:21pm
    Author:  No

  ==============================================================================
*/

#include "FixtureValue.h"

FixtureValue::FixtureValue()
{
    values = new HashMap<FixtureParamDefinition*, ChannelValue*>();
}

void FixtureValue::checkParam(FixtureParamDefinition* chan) {
    if (!values->contains(chan)) {
        values->set(chan, new ChannelValue());
    }
}

void FixtureValue::setValue(FixtureParamDefinition* chan, float value) {
    checkParam(chan);
    values->getReference(chan)->value = value;
}

void FixtureValue::setDelay(FixtureParamDefinition* chan, float value) {
    checkParam(chan);
    values->getReference(chan)->delay = value;
}

void FixtureValue::setFade(FixtureParamDefinition* chan, float value) {
    checkParam(chan);
    values->getReference(chan)->fade = value;
}

