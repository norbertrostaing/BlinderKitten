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

class StampAction :
    public Action
{
public:
    StampAction(var params = var());
    ~StampAction();

    enum ActionType { ST_START, ST_STOP, ST_SIZE, ST_FLASH, ST_SWOP, ST_TOGGLE, ST_STOP_INSTANT};
    ActionType actionType;
    IntParameter* targetId;

    FloatParameter* forcedFade;

    void triggerInternal() override;
    void setValueInternal(var value, String origin, int incrementIndex, bool isRelative) override;

    var getValue();

    static StampAction* create(var params) { return new StampAction(params); }

};