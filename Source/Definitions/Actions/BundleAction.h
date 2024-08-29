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

    enum ActionType { BUN_START, BUN_STOP, BUN_SIZE, BUN_SPEED, BUN_DOUBLESPEED, BUN_HALFSPEED, BUN_TAPTEMPO};
    ActionType actionType;
    IntParameter* targetId;
    FloatParameter* maxSpeed;

    BoolParameter* useHTP;
    BoolParameter* useLTP;
    BoolParameter* useSize;
    BoolParameter* useFlash;

    float lastValue = 0;

    void triggerInternal() override;
    void setValueInternal(var value, String origin, bool isRelative);

    var getValue();

    static BundleAction* create(var params) { return new BundleAction(params); }

};