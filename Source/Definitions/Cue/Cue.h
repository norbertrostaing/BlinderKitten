/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "../Command/Command.h"
#include "Task.h"
#include "Command/MoveInBlackManager.h"

class Cuelist;

class Cue:
    public BaseItem,
    public ChangeBroadcaster
{
public:
    Cue(var params = var());
    virtual ~Cue();

    String objectType;
    var objectData;

    Cuelist* parentCuelist = nullptr;

    BaseManager<Command> commands;
    BaseManager<Task> tasks;
    BaseManager<Task> tasksOffCue;
    FloatParameter* id;

    StringParameter* cueText;
    StringParameter* goText;

    EnumParameter* autoFollow = nullptr;
    FloatParameter* autoFollowTiming;
    FloatParameter* autoFollowCountDown;
    BoolParameter* canBeRandomlyCalled;
    BoolParameter* loadWindowBreakLine;
    double TSAutoFollowStart;
    double TSAutoFollowEnd;

    BoolParameter* releaseCurrentTracking;
    TargetParameter* reuseCue;

    ControllableContainer actionsContainer;
    Trigger* goBtn;
    Trigger* loadBtn;
    Trigger* replaceBtn;
    Trigger* mergeBtn;
    Trigger* createAfterBtn;
    Trigger* createBeforeBtn;
    Trigger* cleanUnusedCommandsBtn;

    MoveInBlackManager moveInBlack;
    FloatParameter* moveInBlackDelay;

    bool isComputing = false;

    HashMap<SubFixtureChannel*, std::shared_ptr<ChannelValue>> computedValues;
    HashMap<SubFixtureChannel*, Command*> channelToCommand;
    CriticalSection csComputing;

    Array<Command*> commandHistory;

    void triggerTriggered(Trigger* t) override;
    void onContainerParameterChangedInternal(Parameter* p) override;
    void onControllableFeedbackUpdate(ControllableContainer* cc, Controllable* c);

    void computeValues();
    void computeValues(Array<Cue*> history, Cue* callingCue);
    void update(double now);
    void go();
    void go(float forcedDelay, float forcedFade);
    void off();
    void off(float forcedDelay, float forcedFade);
    void endTransition();

    void cleanUnused();

    void loadContent(Programmer *p);
    void replaceContent(Programmer* p);
    void mergeContent(Programmer* p);

    void writeTimeStamp();

    Array<Task*> getTasks();
    Array<Task*> getTasks(Array<Cue*> history);

    Array<Task*> getOffTasks();
    Array<Task*> getOffTasks(Array<Cue*> history);

    void runTasks(float forcedDelay = -1, float forcedFade = -1);
    void runOffTasks(float forcedDelay =-1, float forcedFade = -1);

    String getCommandsText(bool useName);

    String getTypeString() const override { return objectType; }
    static Cue* create(var params) { return new Cue(params); }
    double maxTiming = 0;
    //void updateChannels();

    FloatParameter* htpInDelay;
    FloatParameter* htpOutDelay;
    FloatParameter* ltpDelay;
    FloatParameter* htpInFade;
    FloatParameter* htpOutFade;
    FloatParameter* ltpFade;

    ControllableContainer timingContainer;
    StringParameter* lastTriggeredTS;

    void onContainerNiceNameChanged() override;

    void checkId();
    void checkParentCuelist();

};

