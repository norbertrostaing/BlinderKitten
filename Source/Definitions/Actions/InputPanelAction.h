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

class InputPanelAction :
    public Action
{
public:
    InputPanelAction(var params);
    ~InputPanelAction();

    enum ActionType { IP_PRESS, IP_GM, IP_KILLCL, IP_OFFCL, IP_STOPFX, IP_STOPCAR, IP_RANDOMSEED, IP_SELECTWINDOW, IP_SAVE, IP_LOADFROMACTIVECUES };
    ActionType actionType;
    EnumParameter* targetButton;
    IntParameter* randomSeed;
    IntParameter* targetEncoder;
    EnumParameter* targetWindow;

    void triggerInternal() override;
    void setValueInternal(var value, String origin, bool isRelative);

    var getValue();

    static InputPanelAction* create(var params) { return new InputPanelAction(params); }

};