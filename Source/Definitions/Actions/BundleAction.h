/*
  ==============================================================================

    CuelistAction.h
    Created: 3 Feb 2022 10:15:35pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include"../../Common/Action/Action.h"

class BundleAction :
    public Action
{
public:
    BundleAction(var params = var());
    ~BundleAction();

    enum ActionType { BUN_START, BUN_STOP, BUN_SIZE, BUN_SET_SIZE, BUN_SPEED, BUN_DOUBLESPEED, BUN_HALFSPEED, BUN_TAPTEMPO, BUN_FLASH, BUN_SWOP, BUN_TIMED_FLASH, BUN_TIMED_SWOP};
    ActionType actionType;
    IntParameter* targetId;
    FloatParameter* maxSpeed;
    FloatParameter* finalSize;

    BoolParameter* useHTP;
    BoolParameter* useLTP;
    BoolParameter* useSize;
    BoolParameter* useFlash;

    float lastValue = 0;

    void triggerInternal() override;
    void setValueInternal(var value, String origin, int incrementIndex, bool isRelative) override;

    var getValue();

    static BundleAction* create(var params) { return new BundleAction(params); }

};