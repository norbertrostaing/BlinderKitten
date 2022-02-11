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
#include "../Cue/CueManager.h"

class Cuelist:
    public BaseItem
{
public:
    Cuelist(var params = var());
    virtual ~Cuelist();

    String objectType;
    var objectData;

    std::unique_ptr<CueManager> cues;

    IntParameter* id;
    StringParameter* userName;
    void updateName();

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

    Automation offFadeCurve;
    FloatParameter* offFade;
    HashMap<SubFixtureChannel*, ChannelValue*> activeValues;

    TargetParameter* currentCue;
    TargetParameter* nextCue;

    FloatParameter* currentFade;

    Cue* cueA;
    Cue* cueB;

    float xFade = 0;
    double currentTimeElapsed = 0;
    bool isOn = false;

    double TSTransitionStart;
    double TSTransitionDuration;
    double TSTransitionEnd;

    String getTypeString() const override { return objectType; }

    void go(Cue* c);
    void go();
    void off();
    void kill(bool forceRefreshChannels = true);
    void update();
    void autoLoadCueB();
    float applyToChannel(SubFixtureChannel* fc, float currentVal, double now);
    void cleanActiveValues();

    void reorderCues();

    void triggerTriggered(Trigger* t) override;
    void onContainerParameterChangedInternal(Parameter* p);

    void updateHTPs();
    void updateLTPs();

    void setHTPLevel(float level); 

    static Cuelist* create(var params) { return new Cuelist(params); }

};

