/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================

*/

#pragma once
#include "JuceHeader.h"
#include "../Command/CommandSelectionManager.h"
#include "TrackerRow.h"
class TrackerStep;
class SubFixtureChannel;

class Tracker :
    public BaseItem
{
public:
    Tracker(var params = var());
    virtual ~Tracker();

    String objectType;
    var objectData;

    IntParameter* id;
    int registeredId = 0;
    StringParameter* userName;
    void onContainerParameterChangedInternal(Parameter* p);
    void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c);
    void updateName();

    BoolParameter* isTrackerOn;
    bool isOn = false;

    Trigger* startBtn;
    Trigger* stopBtn;

    BoolParameter* autoStartAndStop;
    FloatParameter* sizeValue;
    float lastSize = 0;
    BaseManager<TrackerRow> rows;

    HashMap<SubFixtureChannel*, Array<TrackerRow*>*> chanToTrackerRow;

    String getTypeString() const override { return objectType; }

    void triggerTriggered(Trigger* t);
    void update(double now);
    void computeData();
    bool computed = false;
    bool computing = false;
    void pleaseComputeIfRunning();
    void start();
    void stop();
    float applyToChannel(SubFixtureChannel* fc, float currentVal, double now);

    static Tracker* create(var params) { return new Tracker(params); }
    
};
