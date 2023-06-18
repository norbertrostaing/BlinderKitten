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

class TapTempoMultiple :
    public Action
{
public:
    TapTempoMultiple(var params = var());
    ~TapTempoMultiple();

    enum ActionType { TAPTEMPO_CUELIST, TAPTEMPO_FX, TAPTEMPO_CAR };
    ActionType actionType;
    IntParameter* targetIdFrom;
    IntParameter* targetIdTo;

    void triggerInternal() override;
    void setValueInternal(var value, String origin, bool isRelative);

    var getValue();

    static TapTempoMultiple* create(var params) { return new TapTempoMultiple(params); }

};