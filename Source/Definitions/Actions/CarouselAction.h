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

    enum ActionType { CAR_START, CAR_STOP, CAR_SIZE, CAR_SPEED, CAR_DOUBLESPEED, CAR_HALFSPEED, CAR_TAPTEMPO, CAR_TOGGLE, CAR_BBW };
    ActionType actionType;
    IntParameter* targetId;
    FloatParameter* maxSpeed;

    EnumParameter* buddyBlockOrWing;
    EnumParameter* setOrAdd;
    IntParameter* carRow;
    IntParameter* amount;

    void triggerInternal() override;
    void setValueInternal(var value, String origin, bool isRelative);

    var getValue();

    static CarouselAction* create(var params) { return new CarouselAction(params); }

};