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
#include "Definitions/Interface/InterfaceIncludes.h"

class EffectParam;
class SubFixtureChannel;

class Effect :
    public BaseItem,
    public MIDIInterface::ClockListener
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
    void onContainerParameterChangedInternal(Parameter* p) override;
    void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) override;
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

    double maxOffset = 0;

    IntParameter* soloPool;

    HashMap<SubFixtureChannel*, std::shared_ptr<Array<EffectParam*>>> chanToFxParam;
    SpinLock isComputing;

    double lastTapTempo;
    Array<double> tapTempoHistory;
    Trigger* tapTempoBtn;
    FloatParameter* beatPerCycle;

    TargetParameter* midiClockSyncInterface;

    MultiplicatorLinkManager speedMult;
    MultiplicatorLinkManager sizeMult;
    float currentSizeMult = 1;

    String getTypeString() const override { return objectType; }

    void triggerTriggered(Trigger* t) override;
    void update(double now);
    void computeData();
    bool computed = false;
    bool computing = false;
    void pleaseComputeIfRunning();
    void userStart(float forcedFade = -1);
    void start(float forcedFade = -1);
    void stop(float forcedFade = -1);
    void kill();
    float applyToChannel(SubFixtureChannel* fc, float currentVal, double now);
    void tapTempo();

    void midiClockInterfaceChanged();
    MIDIInterface* currentMidiClockSyncInterface = nullptr;
    double lastMidiTick = 0;

    void midiClockTick() override;
    void midiClockStart() override;
    void midiClockContinue() override;
    void midiClockStop() override;

    void flash(bool on, bool swop = false);
    bool userPressedGo = false;

    static Effect* create(var params) { return new Effect(params); }
    
};
