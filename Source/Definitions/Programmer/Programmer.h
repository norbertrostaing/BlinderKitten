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

class Programmer:
    public BaseItem
{
public:
    Programmer(var params = var());
    virtual ~Programmer();

    String objectType;
    var objectData;

    std::unique_ptr<BaseManager<Command>> commands;
    IntParameter* id;

    EnumParameter* editionMode;
    FloatParameter* releaseTime;
    Trigger* goBtn;
    Trigger* releaseBtn;


    HashMap<FixtureChannel*, ChannelValue*> computedValues;
    HashMap<FixtureChannel*, ChannelValue*> activeValues;

    void triggerTriggered(Trigger* t) override;
    void onContainerParameterChangedInternal(Parameter* p) override;

    void computeValues();
    void update(int64 now);

    void render(int64 now);
    void release(int64 now = 0);
    void go();

    float applyToChannel(FixtureChannel* fc, float currentVal, int64 now);

    String getTypeString() const override { return objectType; }
    static Programmer* create(var params) { return new Programmer(params); }
    int64 maxTiming = 0;
    void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) override;
};

