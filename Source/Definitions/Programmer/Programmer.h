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

class Programmer:
    public BaseItem
{
public:
    Programmer(var params = var());
    virtual ~Programmer();

    String objectType;
    var objectData;

    CommandTiming timing;
    BaseManager<Command> commands;

    IntParameter* id;
    IntParameter* layerId;
    int registeredId = 0;
    StringParameter* userName;
    void onContainerParameterChangedInternal(Parameter* p);
    void updateName();
    bool isDeleted = false;

    CriticalSection computing;

    EnumParameter* editionMode;
    FloatParameter* releaseTime = nullptr;
    Trigger* goBtn;
    Trigger* releaseBtn;
    Trigger* recBtn;
    Trigger* clearAllBtn;

    BoolParameter* highlightCurrentCommand;

    HashMap<SubFixtureChannel*, std::shared_ptr<ChannelValue>> computedValues;
    HashMap<SubFixtureChannel*, std::shared_ptr<ChannelValue>> activeValues;

    ControllableContainer cliContainer;
    EnumParameter* cliActionType;
    EnumParameter* cliParamAType;
    IntParameter*  cliParamAId;
    EnumParameter* cliParamBType;
    IntParameter*  cliParamBId;
    Trigger* cliGo;
    void resetCli();
    bool userCanPressAction = false;
    bool userCanPressTargetType = false;
    bool userCanPressNumber = false;
    bool userCanPressGo = false;
    EnumParameter* currentUserTargetType = nullptr;
    IntParameter* currentUserTargetId = nullptr;
    String cliLastTarget = "";
    void runCliCommand();

    StringArray getCliAsTexts();
    void userCantPress();

    void triggerTriggered(Trigger* t) override;

    void computeValues();
    void update(double now);

    void render(double now);
    void release(double now = 0);
    void go();
    void clearAll();
    void clearCurrent();

    void selectNextCommand();
    void selectPrevCommand();
    void selectCommand(Command* c);

    float applyToChannel(SubFixtureChannel* fc, float currentVal, double now);

    String getTypeString() const override { return objectType; }
    static Programmer* create(var params) { return new Programmer(params); }
    double maxTiming = 0;
    void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) override;

    Command* currentUserCommand = nullptr;
    void processUserInput(String s);

    String getTextCommand(bool useNames = false);
    void checkCurrentUserCommand();
};

