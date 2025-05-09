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

class SelectionMasterAction :
    public Action
{
public:
    SelectionMasterAction(var params = var());
    ~SelectionMasterAction();

    enum ActionType { SELM_START, SELM_STOP, SELM_SIZE, SELM_STATIC_SIZE, SELM_TOGGLE};
    ActionType actionType;
    IntParameter* targetId;
    Point2DParameter* sizeRange;
    FloatParameter* staticSize;

    void triggerInternal() override;
    void setValueInternal(var value, String origin, int incrementIndex, bool isRelative) override;

    var getValue();

    static SelectionMasterAction* create(var params) { return new SelectionMasterAction(params); }

};