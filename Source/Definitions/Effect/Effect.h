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
class FixtureChannel;

class Effect :
    public BaseItem
{
public:
    Effect(var params = var());
    virtual ~Effect();

    String objectType;
    var objectData;

    IntParameter* id;

    BoolParameter* isEffectOn;
    bool isOn;

    Trigger* startBtn;
    Trigger* stopBtn;

    FloatParameter* speed;
    FloatParameter* sizeValue;
    BaseManager<EffectRow> values;

    FloatParameter* currentPosition;
    double TSLastUpdate;
    double totalElapsed;

    HashMap<FixtureChannel*, Array<EffectParam*>*> chanToFxParam;

    String getTypeString() const override { return objectType; }

    void parameterValueChanged(Parameter* p);
    void triggerTriggered(Trigger* t);
    void update(double now);
    void computeData();
    void start();
    void stop();
    float applyToChannel(FixtureChannel* fc, float currentVal, double now);

    static Effect* create(var params) { return new Effect(params); }
    
};
