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

    enum ActionType {   CL_GO, CL_GOBACK, CL_GOINSTANT, CL_GOBACKINSTANT, CL_OFF, 
                        CL_HTPLEVEL, CL_LTPLEVEL, CL_FLASHLEVEL, 
                        CL_FLASH, CL_FLASHTIMED, CL_SWOP, CL_SWOPTIMED, 
                        CL_LOAD, CL_LOADANDGO, CL_GORANDOM, CL_TOGGLE, CL_GOALLLOADED, 
                        CL_CHASERSPEED, CL_CHASERTAPTEMPO, 
                        CL_CROSSFADE, CL_UPFADE, CL_DOWNFADE,
                        CL_OFF_INSTANT, 
                        CL_LOADCONTENT, CL_INSERTBEFORE, CL_INSERTAFTER,
                        CL_TAKESELECTION
    };
    ActionType actionType;
    BoolParameter* useMainConductor;
    IntParameter* cuelistId;
    FloatParameter* cueId;
    FloatParameter* maxSpeed;

    void triggerInternal() override;
    void setValueInternal(var value, String origin, int incrementIndex, bool isRelative) override;

    void onContainerParameterChangedInternal(Parameter*);
    void updateDisplay();

    var getValue();

    static CuelistAction* create(var params) { return new CuelistAction(params); }

};