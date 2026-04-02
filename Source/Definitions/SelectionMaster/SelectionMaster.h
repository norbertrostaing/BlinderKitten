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
          
class SubFixtureChannel;

class SelectionMaster :
    public BaseItem
{
public:
    SelectionMaster(var params = var());
    virtual ~SelectionMaster();

    String objectType;
    var objectData;

    IntParameter* id;
    IntParameter* layerId;
    int registeredId = 0;
    StringParameter* userName;
    void onContainerParameterChangedInternal(Parameter* p) override;
    void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) override;
    void updateName();

    enum MasterType{SCALE, INHIBIT, OFFSET};
    EnumParameter* masterType;
    TargetParameter* channelType;
    BoolParameter* isSelectionMasterOn;
    bool isOn = false;

    Trigger* startBtn;
    Trigger* stopBtn;
    BoolParameter* autoStartAndStop;
    FloatParameter* sizeValue;
    String currentSizeController = "";
    String nextSizeController = "";
    float lastSize = 0;

    CommandSelectionManager selection;

    Array<SubFixtureChannel*> channels;
    CriticalSection isComputing;

    String getTypeString() const override { return objectType; }

    void triggerTriggered(Trigger* t) override;
    void update(double now);
    void computeData();
    void updateSelection(bool restart = false);
    bool computed = false;
    bool computing = false;
    void start();
    void stop();
    float applyToChannel(SubFixtureChannel* fc, float currentVal, double now);

    void updateAllSubFixtureChannels();

    static SelectionMaster* create(var params) { return new SelectionMaster(params); }
    
};
