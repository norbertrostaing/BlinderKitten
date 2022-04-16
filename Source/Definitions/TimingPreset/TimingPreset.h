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

// #include "../Command/CommandSelectionManager.h"
class CommandSelectionManager;

class TimingPreset :
    public BaseItem
{
public:
    TimingPreset(var params = var());
    virtual ~TimingPreset();

    String objectType;
    var objectData;

    IntParameter* id;
    int registeredId = 0;
    StringParameter* userName;
    void onContainerParameterChangedInternal(Parameter* p);
    void updateName();

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

    void updateDisplay();

    String getTypeString() const override { return objectType; }

    static TimingPreset* create(var params) { return new TimingPreset(params); }
};
