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
#include "Definitions/Multiplicator/MultiplicatorLinkManager.h"

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
    IntParameter* layerId;
    int registeredId = 0;
    StringParameter* userName;
    void onContainerParameterChangedInternal(Parameter* p);
    void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c);
    void updateName();

    BoolParameter* isEffectOn;
    bool isOn = false;
    bool isFlashing = false;
    bool isSwopping = false;

    Trigger* startBtn;
    Trigger* stopBtn;

    BoolParameter* autoStartAndStop;
    FloatParameter* speed;
    FloatParameter* sizeValue;
    FloatParameter* flashValue;
    BoolParameter* noLoop;
    String currentSizeController = "";
    String nextSizeController = "";
    float lastSize = 0;

    FloatParameter* fadeInTime;
    FloatParameter* fadeOutTime;
    double TSStartFadeIn = 0;
    double TSStartFadeOut = 0;
    double TSEndFadeIn = 0;
    double TSEndFadeOut = 0;

    BaseManager<EffectRow> values;

    FloatParameter* currentPosition;
    double TSLastUpdate;
    double totalElapsed;

    IntParameter* soloPool;

    HashMap<SubFixtureChannel*, std::shared_ptr<Array<EffectParam*>>> chanToFxParam;
    CriticalSection isComputing;

    double lastTapTempo;
    Array<double> tapTempoHistory;
    Trigger* tapTempoBtn;
    IntParameter* beatPerCycle;

    MultiplicatorLinkManager speedMult;
    MultiplicatorLinkManager sizeMult;
    float currentSizeMult = 1;

    String getTypeString() const override { return objectType; }

    void triggerTriggered(Trigger* t);
    void update(double now);
    void computeData();
    bool computed = false;
    bool computing = false;
    void pleaseComputeIfRunning();
    void userStart(bool useFadeIn = true);
    void start(bool useFadeIn = true);
    void stop();
    void kill();
    float applyToChannel(SubFixtureChannel* fc, float currentVal, double now);
    void tapTempo();

    void flash(bool on, bool swop = false);
    bool userPressedGo = false;

    static Effect* create(var params) { return new Effect(params); }
    
};
