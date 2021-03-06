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

class CuelistAction :
    public Action
{
public:
    CuelistAction(var params = var());
    ~CuelistAction();

    enum ActionType { CL_GO, CL_OFF, CL_HTPLEVEL, CL_LTPLEVEL, CL_FLASH, CL_SWOP, CL_LOAD, CL_LOADANDGO, CL_FLASHLEVEL, CL_GORANDOM, CL_TOGGLE, CL_GOALLLOADED};
    ActionType actionType;
    IntParameter* cuelistId;

    void triggerInternal() override;
    void setValueInternal(var value);

    static CuelistAction* create(var params) { return new CuelistAction(params); }

};