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

class TrackerAction :
    public Action
{
public:
    TrackerAction(var params = var());
    ~TrackerAction();

    enum ActionType { TRK_START, TRK_STOP, TRK_SIZE, TRK_TOGGLE, TRK_AXIS};
    ActionType actionType;
    IntParameter* targetId;
    FloatParameter* maxSpeed;

    enum Axis {Xaxis,Yaxis,Zaxis};
    EnumParameter* axis;
    Point2DParameter* axisRange;

    void triggerInternal() override;
    void setValueInternal(var value, String origin, int incrementIndex, bool isRelative) override;

    var getValue() override;

    static TrackerAction* create(var params) { return new TrackerAction(params); }

};
