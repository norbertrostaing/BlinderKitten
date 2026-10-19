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

    enum ActionType { SOLP_RANDOM, SOLP_STOP, SOLP_START, SOLP_SIZE, SOLP_SET_SIZE, SOLP_SPEED, SOLP_DOUBLESPEED, SOLP_HALFSPEED, SOLP_TAPTEMPO, SOLP_FLASH, SOLP_SWOP, SOLP_TIMED_FLASH, SOLP_TIMED_SWOP, SOLP_STOP_INSTANT, SOLP_SET_SPEED, SOLP_FADE, SOLP_FADE_FIXED };
    ActionType actionType;
    IntParameter* targetId;
    BoolParameter* targetIdThru;
    IntParameter* targetIdTo;

    FloatParameter* maxSpeed;
    FloatParameter* finalSize;
    FloatParameter* finalSpeed;
    BoolParameter* adaptSpeedTapTempo;

    BoolParameter* useHTP;
    BoolParameter* useLTP;
    BoolParameter* useSize;
    BoolParameter* useFlash;

    FloatParameter* forcedFade;

    BoolParameter* inFade;
    BoolParameter* outFade;

    FloatParameter* fadeDurationFixed;
    FloatParameter* fadeDurationFrom;
    FloatParameter* fadeDurationTo;



    float lastValue = 0;

    void triggerInternal() override;
    void setValueInternal(var value, String origin, int incrementIndex, bool isRelative) override;

    void onContainerParameterChangedInternal(Parameter*);
    void updateDisplay();

    var getValue();

    static SoloPoolAction* create(var params) { return new SoloPoolAction(params); }

};