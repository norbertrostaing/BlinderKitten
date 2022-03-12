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

    enum ActionType { VB_PRESS };
    ActionType actionType;
    IntParameter* pageNumber;
    IntParameter* rowNumber;
    IntParameter* colNumber;

    void triggerInternal() override;
    void setValueInternal(var value);

    static VirtualButtonAction* create(var params) { return new VirtualButtonAction(params); }

};