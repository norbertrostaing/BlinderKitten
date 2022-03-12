/*
  ==============================================================================

    VirtualFaderAction.h
    Created: 3 Feb 2022 10:15:35pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include"../../Common/Action/Action.h"

class VirtualFaderAction :
    public Action
{
public:
    VirtualFaderAction(var params);
    ~VirtualFaderAction();

    enum ActionType { VF_FADER, VF_ENCODER, VF_ABOVEBUTTON, VF_BELOWBUTTON};
    ActionType actionType;
    IntParameter* pageNumber;
    IntParameter* colNumber;
    IntParameter* elementNumber;

    void triggerInternal() override;
    void setValueInternal(var value);

    static VirtualFaderAction* create(var params) { return new VirtualFaderAction(params); }

};