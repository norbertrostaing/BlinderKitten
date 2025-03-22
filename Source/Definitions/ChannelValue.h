/*
  ==============================================================================

    SubFixtureValue.h
    Created: 17 Nov 2021 4:07:21pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Command/Command.h"

class ChannelValue {
public:
    ChannelValue();
    ~ChannelValue();

    float value=-0;
    float delay=0;
    float fade=0;

    Array<float> values;

    float startValue();
    float endValue();
    float valueAt(double ratio, double baseVal);

    int64 TSInit = 0;
    int64 TSStart = 0;
    int64 TSEnd = 0;

    bool invertCurve = false;
    Automation* fadeCurve = nullptr;

    bool isEnded = false;
    bool isOverWritten = false;
    float currentPosition = 0;

    bool isTransitionOut = false;
    bool htpOverride = false;

    bool canBeTracked = true;
    bool moveInBlack = true;
    float moveInBlackDelay = 0;

    Command* parentCommand = nullptr;
};
