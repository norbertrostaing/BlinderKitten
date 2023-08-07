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

    enum ActionType { CL_GO, CL_GOBACK, CL_GOINSTANT, CL_GOBACKINSTANT, CL_OFF, CL_HTPLEVEL, CL_LTPLEVEL, CL_FLASH, CL_FLASHTIMED, CL_SWOP, CL_SWOPTIMED, CL_LOAD, CL_LOADANDGO, CL_FLASHLEVEL, CL_GORANDOM, CL_TOGGLE, CL_GOALLLOADED, CL_CHASERSPEED, CL_CHASERTAPTEMPO};
    ActionType actionType;
    IntParameter* cuelistId;
    FloatParameter* cueId;
    FloatParameter* maxSpeed;

    void triggerInternal() override;
    void setValueInternal(var value, String origin, bool isRelative);

    var getValue();

    static CuelistAction* create(var params) { return new CuelistAction(params); }

};