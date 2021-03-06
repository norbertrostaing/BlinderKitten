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

    enum ActionType { IP_PRESS, IP_GM };
    ActionType actionType;
    EnumParameter* targetButton;


    void triggerInternal() override;
    void setValueInternal(var value);

    static InputPanelAction* create(var params) { return new InputPanelAction(params); }

};