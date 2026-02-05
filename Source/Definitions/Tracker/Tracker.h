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
    IntParameter* layerId;
    int registeredId = 0;
    StringParameter* userName;
    void onContainerParameterChangedInternal(Parameter* p) override;
    void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) override;
    void updateName();

    BoolParameter* isTrackerOn;
    bool isOn = false;

    Trigger* startBtn;
    Trigger* stopBtn;

    BoolParameter* autoStartAndStop;
    FloatParameter* sizeValue;
    String currentSizeController = "";
    String nextSizeController = "";
    float lastSize = 0;

    Point3DParameter* targetPosition;
    CommandSelectionManager selection;

    HashMap<SubFixtureChannel*, float> chanToVal;
    CriticalSection isComputing;

    String getTypeString() const override { return objectType; }

    void triggerTriggered(Trigger* t) override;
    void update(double now);
    void computeData();
    bool computed = false;
    bool computing = false;
    void pleaseComputeIfRunning();
    void start();
    void stop();
    float applyToChannel(SubFixtureChannel* fc, float currentVal, double now);

    void updateAllSubFixtures();

    static Tracker* create(var params) { return new Tracker(params); }
    
};
