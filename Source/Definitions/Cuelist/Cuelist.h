/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "../Cue/Cue.h"

class Cuelist:
    public BaseItem
{
public:
    Cuelist(var params = var());
    virtual ~Cuelist();

    String objectType;
    var objectData;

    std::unique_ptr<BaseManager<Cue>> cues;

    IntParameter* id;
    Trigger* goBtn;
    Trigger* offBtn;
    Trigger* killBtn;

    BoolParameter * isCuelistOn;

    FloatParameter* HTPLevel;
    FloatParameter* LTPLevel;
    EnumParameter* tracking;
    EnumParameter* endAction;
    BoolParameter* loopTracking;
    BoolParameter* offIfOverwritten;

    FloatParameter* nextCueId;

    Automation* offFadeCurve;
    FloatParameter* offFade;
    HashMap<FixtureChannel*, ChannelValue*> activeValues;

    TargetParameter* currentCue;
    TargetParameter* nextCue;

    FloatParameter* currentFade;

    Cue* cueA;
    Cue* cueB;

    float xFade = 0;
    int64 currentTimeElapsed = 0;
    bool isOn = false;

    int64 TSTransitionStart;
    int64 TSTransitionDuration;
    int64 TSTransitionEnd;

    String getTypeString() const override { return objectType; }

    void go(Cue* c);
    void go();
    void off();
    void kill(bool forceRefreshChannels = true);
    void update();
    void autoLoadCueB();
    float applyToChannel(FixtureChannel* fc, float currentVal, int64 now);
    void cleanActiveValues();

    void triggerTriggered(Trigger* t) override;
    void onContainerParameterChangedInternal(Parameter* p);

    void updateHTPs();
    void updateLTPs();

    void setHTPLevel(float level); 

    static Cuelist* create(var params) { return new Cuelist(params); }

};

