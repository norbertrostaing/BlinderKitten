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
#include "EffectRow.h"
class EffectParam;
class SubFixtureChannel;

class Effect :
    public BaseItem
{
public:
    Effect(var params = var());
    virtual ~Effect();

    String objectType;
    var objectData;

    IntParameter* id;
    StringParameter* userName;
    void onContainerParameterChangedInternal(Parameter* p);
    void updateName();

    BoolParameter* isEffectOn;
    bool isOn = false;

    Trigger* startBtn;
    Trigger* stopBtn;

    FloatParameter* speed;
    FloatParameter* sizeValue;
    BaseManager<EffectRow> values;

    FloatParameter* currentPosition;
    double TSLastUpdate;
    double totalElapsed;

    HashMap<SubFixtureChannel*, Array<EffectParam*>*> chanToFxParam;

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

    static Effect* create(var params) { return new Effect(params); }
    
};
