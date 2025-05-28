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
#include "Definitions/Command/CommandValueManager.h"
#include "Command/MoveInBlackManager.h"

class Cuelist:
    public BaseItem,
    public ChangeBroadcaster
{
public:
    Cuelist(var params = var());
    virtual ~Cuelist();

    String objectType;
    var objectData;

    CueManager cues;
    bool isDeleting = false;
    bool transitionRunning = false;

    double lastGoTS = 0;

    IntParameter* id;
    IntParameter* layerId;
    int registeredId = 0;
    StringParameter* userName;
    void updateName();

    IntParameter* soloPool;
    
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

    MultiplicatorLinkManager speedMult;


    ControllableContainer chaserGenContainer;
    IntParameter* chaseGenGroup;
    CommandValueManager chaseGenValue;
    IntParameter* chaseGenBuddying;
    IntParameter* chaseGenBlocks;
    IntParameter* chaseGenWings;
    BoolParameter* chaseGenFixturesOnly;
    Trigger* chaserGenButton;

    double chaserStepDuration = 1000;
    double chaserFadeInDuration = 0;
    double chaserFadeOutDuration = 0;
    int chaserIsGoingBackward = false;
    int chaserRemainingSteps = 0;
    void tapTempo();
    double lastTapTempo;
    Array<double> tapTempoHistory;

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

    Trigger* tempMergeTrack;
    Trigger* tempMergeNoTrack;

    Trigger* loadBtn;
    Trigger* loadAndGoBtn;
    Trigger* loadRandomBtn;
    Trigger* cleanAllBtn;
    Trigger* regroupAllBtn;

    Trigger* selectAsMainConductorBtn;

    BoolParameter * isCuelistOn;

    FloatParameter* HTPLevel;
    FloatParameter* LTPLevel;
    float lastHTPLevel = -1;
    float lastLTPLevel = -1;
    FloatParameter* flashLevel;
    BoolParameter* flashWithLtpLevel;
    BoolParameter* absoluteLTPValue;

    String currentHTPLevelController = "";
    String currentLTPLevelController = "";
    String currentFlashLevelController = "";
    String currentCrossFadeController = "";
    String currentUpFadeController = "";
    String currentDownFadeController = "";

    String nextHTPLevelController = "";
    String nextLTPLevelController = "";
    String nextFlashLevelController = "";
    String nextCrossFadeController = "";
    String nextUpFadeController = "";
    String nextDownFadeController = "";

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
    BoolParameter* excludeFromGrandMaster;
    BoolParameter* excludeFromSwop;

    FloatParameter* nextCueId;

    Automation offFadeCurve;
    FloatParameter* offFade;
    HashMap<SubFixtureChannel*, std::shared_ptr<ChannelValue>> activeValues;
    HashMap<SubFixtureChannel*, std::shared_ptr<ChannelValue>> flashingValues;
    Array<Command*> commandHistory;
    CriticalSection isComputing;

    TargetParameter* currentCue;
    TargetParameter* nextCue;

    FloatParameter* currentFade;
    FloatParameter* upFadeController;
    FloatParameter* downFadeController;
    FloatParameter* crossFadeController;
    bool upFadeCanMove = true;
    bool downFadeCanMove = true;
    bool crossFadeCanMove = true;

    float stuckUpFade = 0;
    float stuckDownFade = 0;

    Trigger* renumberCuesBtn;

    Cue* cueA = nullptr;
    Cue* cueB = nullptr;

    ControllableContainer conductorInfos;
    StringParameter* conductorCurrentCueName;
    StringParameter* conductorCurrentCueText;
    FloatParameter* conductorCurrentCueId;
    StringParameter* conductorNextCueGo;
    StringParameter* conductorNextCueName;
    FloatParameter* conductorNextCueId;

    float xFade = 0;
    double currentTimeElapsed = 0;

    double TSTransitionStart;
    double TSTransitionDuration;
    double TSTransitionEnd;
    double TSLateCompensation = 0;

    double TSOffFlash = 0;
    double TSOffFlashEnd = 0;

    CommandTiming timing;
    MoveInBlackManager moveInBlack;
    FloatParameter* moveInBlackDelay;

    String getTypeString() const override { return objectType; }

    void userGo();
    void userGo(Cue* c);
    void userGo(float forcedDelay, float forcedFade);
    void userGo(Cue* c, float forcedDelay, float forcedFade);

    void go();
    void go(Cue* c);
    void go(float forcedDelay, float forcedFade);
    void go(Cue* c, float forcedDelay, float forcedFade);
    void goBack();
    void goBack(float forcedDelay, float forcedFade);
    void goRandom();
    void flash(bool on, bool timing, bool swop = false);
    bool userPressedGo = false;
    bool isFlashing = false;
    bool isSwopping = false;
    bool wannaOffFlash = false;

    void toggle();

    void off(float forcedDelay, float forcedFade);
    void off();
    void kill(bool forceRefreshChannels = true);
    void update();
    void autoLoadCueB();
    float applyToChannel(SubFixtureChannel* fc, float currentVal, double now, bool& isApplied, bool flash = false);

    void reorderCues();

    void triggerTriggered(Trigger* t) override;
    void onContainerParameterChangedInternal(Parameter* p);
    void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c);

    void insertProgCueBefore();
    void insertProgCueAfter();
    void insertProgCueBefore(Cue* c);
    void insertProgCueAfter(Cue* c);
    void insertProgCueAtIndex(int index);

    void updateHTPs();
    void updateLTPs();
    void updateAllChannels();

    void renumberCues();

    void setHTPLevel(float level);
    void setLTPLevel(float level);
    void setFlashLevel(float level);
    void setChaserSpeed(float level);

    bool stopTransition = false;
    double currentManualInTransition = 0;
    double currentManualOutTransition = 0;
    void manualTransition(float ratioIn, float ratioOut);

    void showLoad();
    void showLoadAndGo();
    void loadRandom();

    void fillTexts();
    Cue* getNextCue();

    Cue* getNextChaserCue();
    Cue* getCueAfterId(float id);

    void autoCreateChaser();

    void loadContent();
    void loadContent(Programmer* p);
    void tempMergeProgrammer(Programmer* p, bool trackValues = true);

    void forceCueId(Cue*c, float id);
    void selectAsMainConductor();

    void mergeWithProgrammer(Programmer* p);
    void replaceWithProgrammer(Programmer* p);

    void exportInTextFile();

    void takeSelection(Programmer* p);

    static Cuelist* create(var params) { return new Cuelist(params); }

};

