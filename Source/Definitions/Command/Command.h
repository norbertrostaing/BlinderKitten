/*
  ==============================================================================
 
    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "CommandValue.h"
#include "CommandTiming.h"
#include "CommandValueManager.h"
#include "CommandSelectionManager.h"
#include "MoveInBlackManager.h"

class CommandSelectionManager;

class CommandTiming;
class ChannelValue;
class SubFixtureChannel;
class Cuelist;
class Cue;
class CommandSelection;
class ChannelType;

class Command:
    public BaseItem
{
public:
    Command(var params = var());
    virtual ~Command();

    String objectType;
    var objectData;

    BoolParameter* doNotTrack;
    MoveInBlackManager moveInBlack;
    FloatParameter* moveInBlackDelay;
    BoolParameter* useValuesAsPath;

    CommandSelectionManager selection;
    CommandValueManager values;
    CommandTiming timing;

    HashMap<SubFixtureChannel*, std::shared_ptr<ChannelValue>> computedValues;
    HashMap<SubFixtureChannel*, CommandValue*> channelToCommandValue;
    CriticalSection isComputing;
    bool toDelete = false;

    String getTypeString() const override { return objectType; }
    void parameterValueChanged(Parameter* p) override;
    void afterLoadJSONDataInternal() override;
    void updateDisplay();

    double maxTiming;
    void computeValues();
    void computeValues(Cuelist* cl, Cue* c, Programmer* p = nullptr);

    void onControllableFeedbackUpdate(ControllableContainer*, Controllable*) override;
    void triggerTriggered(Trigger* t) override;
    static Command* create(var params) { return new Command(params); }

    StringArray getCommandAsTexts(bool useNames = false);
    StringArray getCommandSelectionAsTexts();
    String getUserName(String type, int id);
    //Trigger* viewCommandBtn;
    Trigger* explodeSelectionBtn;
    String formatValue(float f);

    bool userCanPressPlusOrMinus = true;
    bool userCanPressSelectionType = true;
    bool userCanPressSubSelection = false;
    bool userCanPressValueType = false;
    bool userCanPressTimingType = false;
    bool userCanPressThru = false;
    bool userCanPressNumber = false;
    bool userCanPressValue = false;
    bool userCanPressSym = false;
    bool userCanHaveAnotherCommand = false;
    void userCantPress();
    void userPress(String s);

    CommandSelection* currentUserSelection = nullptr;
    CommandValue* currentUserValue = nullptr;
    Parameter* currentUserTarget = nullptr;
    BoolParameter* currentUserThru = nullptr;

    float getChannelValue(ChannelType* t, bool thru = false);
    String lastTarget = "";

    void explodeSelection(bool takeOutputValue = false);
    void cleanUnused();

};

