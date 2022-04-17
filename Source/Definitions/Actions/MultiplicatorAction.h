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

class MultiplicatorAction :
    public Action
{
public:
    MultiplicatorAction(var params = var());
    ~MultiplicatorAction();

    enum ActionType { MULT_SETAT, MULT_SET};
    ActionType actionType;
    TargetParameter* targetMult;
    FloatParameter* fromValue;
    FloatParameter* toValue;
    FloatParameter* atValue;

    void triggerInternal() override;
    void setValueInternal(var value);

    static MultiplicatorAction* create(var params) { return new MultiplicatorAction(params); }

};