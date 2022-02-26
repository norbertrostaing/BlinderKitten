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

    CueManager cues;

    IntParameter* id;
    int registeredId = 0;
    StringParameter* userName;
    void updateName();

    Trigger* goBtn;
    Trigger* goRandomBtn;
    Trigger* flashOnBtn;
    Trigger* flashOffBtn;
    Trigger* swopOnBtn;
    Trigger* swopOffBtn;
    Trigger* offBtn;
    Trigger* killBtn;

    BoolParameter * isCuelistOn;

    FloatParameter* HTPLevel;
    FloatParameter* FlashLevel;
    FloatParameter* LTPLevel;
    bool pleaseUpdateHTPs = false;
    bool wannaOff = false;
    EnumParameter* tracking;
    EnumParameter* endAction;
    BoolParameter* loopTracking;
    BoolParameter* offIfOverwritten;
    BoolParameter* isRandom;
    BoolParameter* autoStart;
    BoolParameter* autoStop;

    FloatParameter* nextCueId;

    Automation offFadeCurve;
    FloatParameter* offFade;
    HashMap<SubFixtureChannel*, ChannelValue*> activeValues;
    HashMap<SubFixtureChannel*, ChannelValue*> flashingValues;

    TargetParameter* currentCue;
    TargetParameter* nextCue;

    FloatParameter* currentFade;

    Trigger* renumberCuesBtn;

    Cue* cueA;
    Cue* cueB;

    float xFade = 0;
    double currentTimeElapsed = 0;

    double TSTransitionStart;
    double TSTransitionDuration;
    double TSTransitionEnd;

    String getTypeString() const override { return objectType; }

    void go(Cue* c);
    void go();
    void goRandom();
    void flash(bool on, bool timing, bool swop = false);
    bool isFlashing;
    bool isSwopping;
    bool wannaOffFlash = false;

    void off();
    void kill(bool forceRefreshChannels = true);
    void update();
    void autoLoadCueB();
    float applyToChannel(SubFixtureChannel* fc, float currentVal, double now, bool flash = false);
    void cleanActiveValues();

    void reorderCues();

    void triggerTriggered(Trigger* t) override;
    void onContainerParameterChangedInternal(Parameter* p);

    void updateHTPs();
    void updateLTPs();

    void renumberCues();

    void setHTPLevel(float level);
    void setFlashLevel(float level);

    static Cuelist* create(var params) { return new Cuelist(params); }

};

