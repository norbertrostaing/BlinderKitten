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

class MapperAction :
    public Action
{
public:
    MapperAction(var params = var());
    ~MapperAction();

    enum ActionType { TRK_START, TRK_STOP, TRK_SIZE, TRK_TOGGLE};
    ActionType actionType;
    IntParameter* targetId;
    FloatParameter* maxSpeed;

    void triggerInternal() override;
    void setValueInternal(var value, String origin);

    static MapperAction* create(var params) { return new MapperAction(params); }

};