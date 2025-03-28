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

class PresetAction :
    public Action
{
public:
    PresetAction(var params = var());
    ~PresetAction();

    enum ActionType { TIMING_PRESET_SET, TIMING_PRESET_FIXED};
    ActionType actionType;
    IntParameter* targetId;
    EnumParameter* component;
    FloatParameter* fromValue;
    FloatParameter* toValue;
    FloatParameter* fixedValue;

    void onContainerParameterChangedInternal(Parameter*);
    void updateDisplay();

    void triggerInternal() override;
    void setValueInternal(var value, String origin, int incrementIndex, bool isRelative) override;

    var getValue();

    static PresetAction* create(var params) { return new PresetAction(params); }
};