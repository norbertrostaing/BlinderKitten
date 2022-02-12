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
    CuelistAction(var params);
    ~CuelistAction();

    enum ActionType { CL_GO, CL_OFF, CL_HTPLEVEL, CL_FLASH, CL_FLASHLEVEL, CL_GORANDOM
    };
    ActionType actionType;
    IntParameter* cuelistId;

    void triggerInternal() override;
    void setValueInternal(var value);

    static CuelistAction* create(var params) { return new CuelistAction(params); }

};