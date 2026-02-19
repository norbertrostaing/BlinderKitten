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
    EffectAction(var params = var());
    ~EffectAction();

    enum ActionType { FX_START, FX_STOP, FX_SIZE, FX_FLASH, FX_SWOP, FX_SPEED, FX_DOUBLESPEED, FX_HALFSPEED, FX_TAPTEMPO, FX_TOGGLE, FX_BBW, FX_STOP_INSTANT, FX_SET_SEEK, FX_ADD_SEEK };
    ActionType actionType;
    IntParameter* targetId;
    FloatParameter* maxSpeed;

    EnumParameter* buddyBlockOrWing;
    EnumParameter* setOrAdd;
    IntParameter* fxRow;
    IntParameter* fxParam;
    IntParameter* amount;

    FloatParameter* forcedFade;

    FloatParameter* seekValue;

    void triggerInternal() override;
    void setValueInternal(var value, String origin, int incrementIndex, bool isRelative) override;

    var getValue() override;

    static EffectAction* create(var params) { return new EffectAction(params); }

};
