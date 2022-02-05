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

    std::unique_ptr<BaseManager<Command>> commands;
    FloatParameter* id;
    FloatParameter* autoGoTime;

    Trigger* goBtn;

    HashMap<FixtureChannel*, ChannelValue*> computedValues;

    void parameterValueChanged(Parameter* p) override;
    void triggerTriggered(Trigger* t) override;

    void computeValues();

    String getTypeString() const override { return objectType; }
    static Cue* create(var params) { return new Cue(params); }
    int64 maxTiming = 0;
    void updateChannels();

};

