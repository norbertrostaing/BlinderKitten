/*
  ==============================================================================

    RunningTask.cpp
    Created: 18 Nov 2021 10:10:30pm
    Author:  No

  ==============================================================================
*/

#include "RunningTask.h"
#include "Definitions/Cue/Task.h"

RunningTask::RunningTask() {};
RunningTask::~RunningTask() {};

void RunningTask::update(double now) {
    if (isEnded) { return; }
    if (now >= TSEnd) {
        isEnded = true;
        trigger(endValue);
    }
    else if (now >= TSStart) {
        double val = (now - TSStart) / (TSEnd - TSStart);
        val = jmap(val, startValue, endValue);
        trigger(val);
    }
}

void RunningTask::trigger(double value) {
    Task::triggerGivenTask(targetType, targetId, actionType, value);
}