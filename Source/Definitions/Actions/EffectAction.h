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

class EffectAction :
    public Action
{
public:
    EffectAction(var params);
    ~EffectAction();

    enum ActionType { FX_START, FX_STOP, FX_SIZE, FX_SPEED, FX_TAPTEMPO };
    ActionType actionType;
    IntParameter* targetId;
    FloatParameter* maxSpeed;

    void triggerInternal() override;
    void setValueInternal(var value);

    static EffectAction* create(var params) { return new EffectAction(params); }

};