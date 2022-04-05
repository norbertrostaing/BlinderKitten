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
#include "MapperRow.h"
class MapperStep;
class SubFixtureChannel;

class Mapper :
    public BaseItem
{
public:
    Mapper(var params = var());
    virtual ~Mapper();

    String objectType;
    var objectData;

    IntParameter* id;
    int registeredId = 0;
    StringParameter* userName;
    void onContainerParameterChangedInternal(Parameter* p);
    void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c);
    void updateName();

    BoolParameter* isMapperOn;
    bool isOn = false;

    Trigger* startBtn;
    Trigger* stopBtn;

    BoolParameter* autoStartAndStop;
    FloatParameter* sizeValue;
    float lastSize = 0;
    BaseManager<MapperRow> rows;

    HashMap<SubFixtureChannel*, Array<MapperRow*>*> chanToMapperRow;

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

    static Mapper* create(var params) { return new Mapper(params); }
    
};
