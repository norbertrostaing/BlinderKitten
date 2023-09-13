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

    enum ActionType { STMP_START, STMP_STOP, STMP_TOGGLE, STMP_SIZE};
    ActionType actionType;
    IntParameter* targetId;

    void triggerInternal() override;
    void setValueInternal(var value, String origin, bool isRelative);

    var getValue();

    static StampAction* create(var params) { return new StampAction(params); }

};