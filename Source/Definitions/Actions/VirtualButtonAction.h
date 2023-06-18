/*
  ==============================================================================

    VirtualButtonAction.h
    Created: 3 Feb 2022 10:15:35pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include"../../Common/Action/Action.h"

class VirtualButtonAction :
    public Action
{
public:
    VirtualButtonAction(var params);
    ~VirtualButtonAction();

    enum ActionType { VB_PRESS, VB_PAGEUP, VB_PAGEDOWN, VB_GOTOPAGE };
    ActionType actionType;
    IntParameter* pageNumber;
    IntParameter* rowNumber;
    IntParameter* colNumber;
    IntParameter* goToPageId;

    void triggerInternal() override;
    void setValueInternal(var value, String origin, bool isRelative);

    var getValue();

    static VirtualButtonAction* create(var params) { return new VirtualButtonAction(params); }

};