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

    enum ActionType { VF_FADER, VF_ROTARY, VF_ABOVEBUTTON, VF_BELOWBUTTON, VF_PAGEUP, VF_PAGEDOWN, VF_GOTOPAGE};
    ActionType actionType;
    IntParameter* pageNumber;
    IntParameter* colNumber;
    IntParameter* elementNumber;
    IntParameter* goToPageId;

    void triggerInternal() override;
    void setValueInternal(var value, String origin, int incrementIndex, bool isRelative) override;

    var getValue();

    static VirtualFaderAction* create(var params) { return new VirtualFaderAction(params); }

};