/*
  ==============================================================================

    ChannelType.h
    Created: 7 Nov 2021 7:40:48pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "../Command/CommandValueManager.h"

class SubFixtureChannel;
class SubFixture;
class ChannelValue;

class CarouselStep:
    public BaseItem
{
    public:
    CarouselStep(var params = var());
    ~CarouselStep();

    String objectType;
    var objectData;

    float relativeStartPosition = 0;
    float relativeDuration = 0;

    FloatParameter* stepDuration;
    FloatParameter* fadeRatio;
    Automation curve;
    CommandValueManager values;

    void computeValues(Array<SubFixture*> SubFixtures);
    HashMap<SubFixtureChannel*, ChannelValue*> computedValues;

    void onContainerParameterChangedInternal(Parameter*);
    void updateDisplay();
};