/*
  ==============================================================================

    SubFixtureValue.h
    Created: 17 Nov 2021 4:07:21pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class RunningTask {
public:
    RunningTask();
    ~RunningTask();

    float value=-0;
    float delay=0;
    float fade=0;

    float startValue = -1;
    float endValue = -1;

    int64 TSInit = 0;
    int64 TSStart = 0;
    int64 TSEnd = 0;
    
    bool isEnded = false;
    float currentPosition = 0;

    String targetType = "";
    int targetId = 0;
    String actionType = "";
};
