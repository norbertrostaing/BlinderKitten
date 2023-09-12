/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "../Command/CommandValueManager.h"
#include "../Command/Command.h" 
#include "ChannelValue.h"

class SubFixtureChannel;

class Stamp :
    public ChangeBroadcaster,
    public BaseItem
{
public:
    Stamp(var params = var());
    virtual ~Stamp();

    String objectType;
    var objectData;

    IntParameter* id;
    int registeredId = 0;
    StringParameter* userName;

    IntParameter* layerId;

    BoolParameter* isStampOn;
    bool isOn = false;

    Trigger* startBtn;
    Trigger* stopBtn;

    BoolParameter* autoStartAndStop;
    FloatParameter* sizeValue;

    IntParameter* groupId;
    IntParameter* layoutId;
    IntParameter* mediaId;
    Point2DParameter* position;
    Point2DParameter* dimensions;
    FloatParameter* angle;

    Point2DParameter* redRange;
    Point2DParameter* greenRange;
    Point2DParameter* blueRange;

    Command tempCommand;
    ControllableContainer stampValues;
    CommandValueManager redFull;
    CommandValueManager redZero;
    CommandValueManager greenFull;
    CommandValueManager greenZero;
    CommandValueManager blueFull;
    CommandValueManager blueZero;

    void onContainerParameterChangedInternal(Parameter* p);
    void updateName();
    void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c);

    CriticalSection isComputing;
    void computeData();
    HashMap<SubFixtureChannel*, std::shared_ptr<ChannelValue>> redZeroMap;
    HashMap<SubFixtureChannel*, std::shared_ptr<ChannelValue>> redFullMap;
    HashMap<SubFixtureChannel*, std::shared_ptr<ChannelValue>> greenZeroMap;
    HashMap<SubFixtureChannel*, std::shared_ptr<ChannelValue>> greenFullMap;
    HashMap<SubFixtureChannel*, std::shared_ptr<ChannelValue>> blueZeroMap;
    HashMap<SubFixtureChannel*, std::shared_ptr<ChannelValue>> blueFullMap;
    Array<SubFixtureChannel*> subFixtureChannels;

    void triggerTriggered(Trigger* t);
    void start();
    void stop();

    float applyToChannel(SubFixtureChannel* fc, float currentVal, double now);

    String getTypeString() const override { return objectType; }
    static Stamp* create(var params) { return new Stamp(params); }
};
