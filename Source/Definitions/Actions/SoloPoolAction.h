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

class SoloPoolAction :
    public Action
{
public:
    SoloPoolAction(var params = var());
    ~SoloPoolAction();

    enum ActionType { SOLP_RANDOM, SOLP_STOP};
    ActionType actionType;
    IntParameter* targetId;
    BoolParameter* targetIdThru;
    IntParameter* targetIdTo;

    float lastValue = 0;

    void triggerInternal() override;
    void setValueInternal(var value, String origin, int incrementIndex, bool isRelative) override;

    void onContainerParameterChangedInternal(Parameter*);
    void updateDisplay();

    var getValue();

    static SoloPoolAction* create(var params) { return new SoloPoolAction(params); }

};