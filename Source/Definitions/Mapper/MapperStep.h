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

class MapperStep:
    public BaseItem
{
    public:
    MapperStep(var params = var());
    ~MapperStep();

    String objectType;
    var objectData;

    float relativeStartPosition = 0;
    float relativeDuration = 0;

    FloatParameter* stepValue;
    //FloatParameter* stepFade;
    Automation curve;
    CommandValueManager values;

    void computeValues(Array<SubFixture*> SubFixtures);
    HashMap<SubFixtureChannel*, ChannelValue*> computedValues;

    void onContainerParameterChangedInternal(Parameter*);
    void updateDisplay();
};