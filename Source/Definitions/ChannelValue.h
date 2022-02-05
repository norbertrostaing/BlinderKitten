/*
  ==============================================================================

    FixtureValue.h
    Created: 17 Nov 2021 4:07:21pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class ChannelValue {
public:
    ChannelValue();
    ~ChannelValue();

    float value=-1;
    float delay=0;
    float fade=0;

    float startValue = -1;
    float endValue = -1;

    int64 TSInit = 0;
    int64 TSStart = 0;
    int64 TSEnd = 0;

    bool invertCurve = false;
    Automation* fadeCurve;

    bool isEnded = false;
    bool isOverWritten = false;
    float currentPosition = 0;
};
