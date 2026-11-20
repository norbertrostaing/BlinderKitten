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

    enum ActionType { FX_START, FX_STOP, FX_SIZE, FX_FLASH, FX_SWOP, FX_SPEED, FX_DOUBLESPEED, FX_HALFSPEED, FX_TAPTEMPO, FX_TOGGLE, FX_BBW, FX_STOP_INSTANT, FX_SET_SEEK, FX_ADD_SEEK, FX_SEEK, FX_FLASH_PRESS, FX_FLASH_RELEASE, FX_SWOP_PRESS, FX_SWOP_RELEASE, FX_SPEED_FIXED, FX_SPEED_MULT, FX_SIZE_MULT, FX_SPEED_MULT_FIXED, FX_SIZE_MULT_FIXED
    };
    ActionType actionType;
    IntParameter* targetId;
    FloatParameter* maxSpeed;
    FloatParameter* fixedSpeed;

    EnumParameter* buddyBlockOrWing;
    EnumParameter* setOrAdd;
    IntParameter* fxRow;
    IntParameter* fxParam;
    IntParameter* amount;

    FloatParameter* forcedFade;

    FloatParameter* seekValue;
    FloatParameter* seekFrom;
    FloatParameter* seekTo;

    FloatParameter* multFrom;
    FloatParameter* multTo;
    FloatParameter* multFixed;

    void triggerInternal() override;
    void setValueInternal(var value, String origin, int incrementIndex, bool isRelative) override;

    var getValue();

    static EffectAction* create(var params) { return new EffectAction(params); }

};