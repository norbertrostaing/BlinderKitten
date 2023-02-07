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
    IntParameter* layerId;
    int registeredId = 0;
    StringParameter* userName;
    void updateName();
    
    ControllableContainer conductorInfos;
    StringParameter* currentCueName;
    StringParameter* currentCueText;
    StringParameter* nextCueGo;
    StringParameter* nextCueName;

    ControllableContainer chaserOptions;
    BoolParameter* isChaser;
    EnumParameter* chaserDirection;
    FloatParameter* chaserSpeed;
    FloatParameter* chaserInFade;
    FloatParameter* chaserOutFade;
    FloatParameter* chaserRunXTimes;
    Automation chaserFadeInCurve;
    Automation chaserFadeOutCurve;
    FloatParameter* chaserStepPerTap;
    Trigger* chaserTapTempo;

    double chaserStepDuration = 1000;
    double chaserFadeInDuration = 0;
    double chaserFadeOutDuration = 0;
    int chaserIsGoingBackward = false;
    int chaserRemainingSteps = 0;
    void tapTempo();
    double lastTapTempo;

    Trigger* goBtn;
    Trigger* goBackBtn;
    Trigger* goRandomBtn;
    Trigger* flashOnBtn;
    Trigger* flashOffBtn;
    Trigger* swopOnBtn;
    Trigger* swopOffBtn;
    Trigger* offBtn;
    Trigger* killBtn;
    Trigger* toggleBtn;

    Trigger* loadBtn;
    Trigger* loadAndGoBtn;
    Trigger* loadRandomBtn;
    BoolParameter * isCuelistOn;

    FloatParameter* HTPLevel;
    FloatParameter* LTPLevel;
    float lastHTPLevel = -1;
    FloatParameter* FlashLevel;
    bool pleaseUpdateHTPs = false;
    bool pleaseUpdateLTPs = false;
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
    CriticalSection isComputing;

    TargetParameter* currentCue;
    TargetParameter* nextCue;

    FloatParameter* currentFade;

    Trigger* renumberCuesBtn;

    Cue* cueA = nullptr;
    Cue* cueB = nullptr;

    float xFade = 0;
    double currentTimeElapsed = 0;

    double TSTransitionStart;
    double TSTransitionDuration;
    double TSTransitionEnd;

    String getTypeString() const override { return objectType; }

    void userGo(Cue* c);
    void userGo();
    void go(Cue* c);
    void go();
    void goBack();
    void goRandom();
    void flash(bool on, bool timing, bool swop = false);
    bool userPressedGo = false;
    bool isFlashing = false;
    bool isSwopping = false;
    bool wannaOffFlash = false;

    void toggle();

    void off();
    void kill(bool forceRefreshChannels = true);
    void update();
    void autoLoadCueB();
    float applyToChannel(SubFixtureChannel* fc, float currentVal, double now, bool flash = false);
    void cleanActiveValues();

    void reorderCues();

    void triggerTriggered(Trigger* t) override;
    void onContainerParameterChangedInternal(Parameter* p);
    void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c);

    void insertProgCueBefore(Cue* c);
    void insertProgCueAfter(Cue* c);
    void insertProgCueAtIndex(int index);

    void updateHTPs();
    void updateLTPs();

    void renumberCues();

    void setHTPLevel(float level);
    void setLTPLevel(float level);
    void setFlashLevel(float level);
    void setChaserSpeed(float level);

    void showLoad();
    void showLoadAndGo();
    void loadRandom();

    void fillTexts();
    Cue* getNextCue();

    Cue* getNextChaserCue();

    static Cuelist* create(var params) { return new Cuelist(params); }

};

