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

    double startValue = -1;
    double endValue = -1;

    int64 TSInit = 0;
    int64 TSStart = 0;
    int64 TSEnd = 0;
    
    bool isEnded = false;
    float currentPosition = 0;

    String targetType = "";
    int targetId = 0;
    String actionType = "";

    void update(double now);
    void trigger(double value = 1);
};
