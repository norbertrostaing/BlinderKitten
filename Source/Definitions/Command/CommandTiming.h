/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Definitions/Multiplicator/MultiplicatorLinkManager.h"

class CommandTiming :
    public ControllableContainer
{
public:
    CommandTiming(var params = var());
    virtual ~CommandTiming();

    String objectType;
    var objectData;

    // TargetParameter* timePreset;

    EnumParameter* presetOrValue;
    IntParameter* presetId;

    FloatParameter* fadeFrom;
    FloatParameter* delayFrom;
    FloatParameter* delayTo;
    BoolParameter* symmetryDelay;

    BoolParameter* thruDelay;
    BoolParameter* thruFade;
    FloatParameter* fadeTo;
    BoolParameter* symmetryFade;

    Automation curveFade;
    Automation curveDelayRepart;
    Automation curveFadeRepart;

    MultiplicatorLinkManager delayMult;
    MultiplicatorLinkManager fadeMult;

    // String getTypeString() const override { return objectType; }
    void parameterValueChanged(Parameter* p);
    void afterLoadJSONDataInternal();
    void updateDisplay();

    static CommandTiming* create(var params) { return new CommandTiming(params); }
};
