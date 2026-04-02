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

class CarouselAction :
    public Action
{
public:
    CarouselAction(var params = var());
    ~CarouselAction();

    enum ActionType { CAR_START, CAR_STOP, CAR_SIZE, CAR_FLASH, CAR_SWOP, CAR_SPEED, CAR_DOUBLESPEED, CAR_HALFSPEED, CAR_TAPTEMPO, CAR_TOGGLE, CAR_BBW, CAR_STOP_INSTANT, CAR_SET_SEEK, CAR_ADD_SEEK };
    ActionType actionType;
    IntParameter* targetId;
    FloatParameter* maxSpeed;

    EnumParameter* buddyBlockOrWing;
    EnumParameter* setOrAdd;
    IntParameter* carRow;
    IntParameter* amount;

    FloatParameter* forcedFade;

    FloatParameter* seekValue;

    void triggerInternal() override;
    void setValueInternal(var value, String origin, int incrementIndex, bool isRelative) override;

    var getValue() override;

    static CarouselAction* create(var params) { return new CarouselAction(params); }

};
