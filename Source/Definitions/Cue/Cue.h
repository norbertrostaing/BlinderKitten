/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "../Command/Command.h"

class Cue:
    public BaseItem
{
public:
    Cue(var params = var());
    virtual ~Cue();

    String objectType;
    var objectData;

    BaseManager<Command> commands;
    FloatParameter* id;

    EnumParameter* autoFollow = nullptr;
    FloatParameter* autoFollowTiming;
    FloatParameter* autoFollowCountDown;
    BoolParameter* canBeRandomlyCalled;
    double TSAutoFollowStart;
    double TSAutoFollowEnd;

    Trigger* goBtn;

    HashMap<SubFixtureChannel*, ChannelValue*> computedValues;

    void triggerTriggered(Trigger* t) override;
    void onContainerParameterChangedInternal(Parameter* p) override;

    void computeValues();
    void update(double now);
    void go();
    void endTransition();

    String getTypeString() const override { return objectType; }
    static Cue* create(var params) { return new Cue(params); }
    double maxTiming = 0;
    void updateChannels();

    FloatParameter* htpInDelay;
    FloatParameter* htpOutDelay;
    FloatParameter* ltpDelay;
    FloatParameter* htpInFade;
    FloatParameter* htpOutFade;
    FloatParameter* ltpFade;

    ControllableContainer timingContainer;

};

