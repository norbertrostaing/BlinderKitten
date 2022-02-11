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
    StringParameter* userName;
    void onContainerParameterChangedInternal(Parameter* p);
    void updateName();

    EnumParameter* editionMode;
    FloatParameter* releaseTime;
    Trigger* goBtn;
    Trigger* releaseBtn;
    Trigger* recBtn;


    HashMap<SubFixtureChannel*, ChannelValue*> computedValues;
    HashMap<SubFixtureChannel*, ChannelValue*> activeValues;

    void triggerTriggered(Trigger* t) override;

    void computeValues();
    void update(double now);

    void render(double now);
    void release(double now = 0);
    void go();

    float applyToChannel(SubFixtureChannel* fc, float currentVal, double now);

    String getTypeString() const override { return objectType; }
    static Programmer* create(var params) { return new Programmer(params); }
    double maxTiming = 0;
    void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) override;
};

