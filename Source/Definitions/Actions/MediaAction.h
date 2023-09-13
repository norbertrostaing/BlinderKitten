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

class MediaAction :
    public Action
{
public:
    MediaAction(var params = var());
    ~MediaAction();

    enum ActionType { MED_START, MED_RESTART, MED_STOP, MED_PAUSE, MED_VOL, MED_RATE, MED_DOUBLESPEED, MED_HALFSPEED, MED_TAPTEMPO};
    ActionType actionType;
    IntParameter* targetId;
    FloatParameter* maxSpeed;


    void triggerInternal() override;
    void setValueInternal(var value, String origin, bool isRelative);

    var getValue();

    static MediaAction* create(var params) { return new MediaAction(params); }

};