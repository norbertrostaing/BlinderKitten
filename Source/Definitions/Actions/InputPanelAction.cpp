/*
  ==============================================================================

    InputPanelAction.cpp
    Created: 3 Feb 2022 10:15:35pm
    Author:  No

  ==============================================================================
*/

#include "InputPanelAction.h"
#include "../Carousel/Carousel.h"
#include "../../Brain.h"
#include "UserInputManager.h"
#include "UI/InputPanel.h"
#include "UI/Encoders.h"

InputPanelAction::InputPanelAction(var params) :
    Action(params)
{
    actionType = (ActionType)(int)params.getProperty("actionType", IP_PRESS);

    if (actionType == IP_PRESS) {
        targetButton = addEnumParameter("Button", "choose which button you wanna press");
        targetButton->addOption("Assistant", "Assistant");
        targetButton->addOption("Fixture", "Fixture");
        targetButton->addOption("Group", "Group");
        targetButton->addOption("Sub", "Sub");
        targetButton->addOption("Cuelist", "Cuelist");
        targetButton->addOption("Cue", "Cue");
        targetButton->addOption("Preset", "Preset");
        targetButton->addOption("Effect", "Effect");
        targetButton->addOption("Carousel", "Carousel");
        targetButton->addOption("1", "1");
        targetButton->addOption("2", "2");
        targetButton->addOption("3", "3");
        targetButton->addOption("4", "4");
        targetButton->addOption("5", "5");
        targetButton->addOption("6", "6");
        targetButton->addOption("7", "7");
        targetButton->addOption("8", "8");
        targetButton->addOption("9", "9");
        targetButton->addOption("0", "0");
        targetButton->addOption("Plus", "Plus");
        targetButton->addOption("Minus", "Minus");
        targetButton->addOption("Dot", "Dot");
        targetButton->addOption("Thru", "Thru");
        targetButton->addOption("Backspace", "Backspace");
        targetButton->addOption("Enter", "Enter");
        targetButton->addOption("Clear", "Clear");
        targetButton->addOption("ClearAll", "ClearAll");
        targetButton->addOption("Record", "Record");
        targetButton->addOption("Merge", "Merge");
        targetButton->addOption("Replace", "Replace");
        targetButton->addOption("Edit", "Edit");
        targetButton->addOption("Load content", "Loadcontent");
        targetButton->addOption("Copy", "Copy");
        targetButton->addOption("Delete", "Delete");
        targetButton->addOption("Black Out", "Blackout");
        targetButton->addOption("Midi Lock", "MidiLock");
    }
    else if (actionType == IP_RANDOMSEED) {
        randomSeed = addIntParameter("Random Seed", "Initialise all random events with this value",0,0);
    }
    else if (actionType == IP_SELECTWINDOW) {
        targetWindow = addEnumParameter("Window", "Wich tab do you want to magically appear ?");
        for (int i = 0; i < ShapeShifterFactory::getInstance()->defs.size(); i++) {
            String windowName = ShapeShifterFactory::getInstance()->defs[i]->contentName;
            targetWindow->addOption(windowName, windowName);
        }
    }
    else if (actionType == IP_ORGANICLAYOUT) {
        organicLayoutId = addIntParameter("Layout Number", "number of the layout", 1,1,10);
    }
    else if (actionType == IP_USEANOTHER) {
        useAnotherTargetType = addEnumParameter("Target type", "");
        useAnotherTargetType->addOption("Fixture", "Fixture")->addOption("Group", "Group")->addOption("Preset", "Preset")->addOption("Timing Preset", "Timing Preset");
        useAnotherTargetId = addIntParameter("Target ID", "", 0,0);
        useAnotherRandomize = addBoolParameter("Randomize", "", false);

        useAnotherOtherId = addIntParameter("Other ID", "", 0,0);
        useAnotherFromId = addIntParameter("Other from", "", 0,0);
        useAnotherToId = addIntParameter("Other to", "", 0,0);
    }

    updateDisplay();
}

InputPanelAction::~InputPanelAction()
{
}

void InputPanelAction::triggerInternal()
{
}

void InputPanelAction::setValueInternal(var value, String origin, int incrementIndex, bool isRelative) {
    float val = value;

    switch (actionType)
    {
    case IP_PRESS:
        if (val == 1) {
            UserInputManager::getInstance()->processInput(targetButton->getValue());
        }
        break;

    case IP_GM:
        {
        const MessageManagerLock mmLock;
        if (isRelative) 
            {
            float v = InputPanel::getInstance()->paramGrandMaster->floatValue() + val;
            v = jlimit<float>(0,1,v);
            
            InputPanel::getInstance()->setGrandMaster(v, origin);
            }
        else 
            {
            InputPanel::getInstance()->setGrandMaster(val, origin);
            }
        }
        break;

    case IP_BLACKOUTTEMP:
    {
        const MessageManagerLock mmLock;

        InputPanel::getInstance()->paramBlackOut->setValue(val == 1);
        break;
    }
    case IP_BLACKOUTTOGGLE:
    {
        const MessageManagerLock mmLock;

        if (val == 1) InputPanel::getInstance()->paramBlackOut->setValue(!InputPanel::getInstance()->paramBlackOut->boolValue());
        break;
    }

    case IP_OFFCL:
        if (val == 1) {
            Brain::getInstance()->offAllCuelists();
        }
        break;

    case IP_KILLCL:
        if (val == 1) {
            Brain::getInstance()->killAllCuelists();
        }
        break;

    case IP_STOPFX:
        if (val == 1) {
            Brain::getInstance()->stopAllEffects();
        }
        break;

    case IP_STOPCAR:
        if (val == 1) {
            Brain::getInstance()->stopAllCarousels();
        }
        break;

    case IP_RANDOMSEED:
        if (val == 1) {
            Brain::getInstance()->resetRandomSeed(randomSeed->getValue());
        }
        break;

    case IP_LOADALLCUELISTS:
        if (val == 1) {
            Brain::getInstance()->loadRunningCuelistsInProgrammer();
        }
        break;

    case IP_SELECTWINDOW:
        if (val == 1) {
            Brain::getInstance()->showWindow(targetWindow->stringValue());
        }
        break;

    case IP_SAVE:
        if (val == 1) {
            MessageManager::callAsync([]() {
                if (Engine::mainEngine->getFile().exists())
                {
                    juce::Result r = Engine::mainEngine->saveDocument(Engine::mainEngine->getFile());
                    if (r.ok()) LOG("Session saved");
                    else if (r.failed()) LOGERROR("Session save error : " << r.getErrorMessage());
                }
                else
                {
                    LOGWARNING("Please save first manually to use the save session command");
                }
                });
        }
        break;
    case IP_USEANOTHER:
        if (val == 1) {
            setAnother();
        }
        break;
    case IP_ORGANICLAYOUT:
        if (val == 1) {
            MessageManager::callAsync([this](){
                int commandId = 0x9000 + organicLayoutId->intValue() - 1;
                ShapeShifterManager::getInstance()->handleCommandID(commandId);
                }
            );
        }
    }


}

void InputPanelAction::setAnother()
{
    int min = useAnotherFromId->intValue();
    int max = useAnotherToId->intValue();
    if (min > max) {
        min = useAnotherToId->intValue();
        max = useAnotherFromId->intValue();
    }
    int newId = 0;
    String type = useAnotherTargetType->stringValue();
    if (type == "Preset") {
        Preset* target = Brain::getInstance()->getPresetById(useAnotherTargetId->intValue());
        if (target == nullptr) { return; }
        if (useAnotherRandomize->boolValue()) {
            Array<Preset*> available;
            for (int i = min; i <= max; i++) {
                Preset* p = Brain::getInstance()->getPresetById(i);
                if (p != nullptr) available.add(p);
            }
            if (available.size() > 0) {
                Random r;
                int index = r.nextInt(available.size());
                newId = available[index]->id->intValue();
            }
        }
        else {
            newId = useAnotherOtherId->intValue();
        }
        target->useAnotherId->setValue(newId);
    }
    else if (type == "Timing Preset") {
        TimingPreset* target = Brain::getInstance()->getTimingPresetById(useAnotherTargetId->intValue());
        if (target == nullptr) { return; }
        if (useAnotherRandomize->boolValue()) {
            Array<TimingPreset*> available;
            for (int i = min; i <= max; i++) {
                TimingPreset* p = Brain::getInstance()->getTimingPresetById(i);
                if (p != nullptr) available.add(p);
            }
            if (available.size() > 0) {
                Random r;
                int index = r.nextInt(available.size());
                newId = available[index]->id->intValue();
            }
        }
        else {
            newId = useAnotherOtherId->intValue();
        }
        target->useAnotherId->setValue(newId);
    }
    else if (type == "Fixture") {
        Fixture* target = Brain::getInstance()->getFixtureById(useAnotherTargetId->intValue());
        if (target == nullptr) { return; }
        if (useAnotherRandomize->boolValue()) {
            Array<Fixture*> available;
            for (int i = min; i <= max; i++) {
                Fixture* p = Brain::getInstance()->getFixtureById(i);
                if (p != nullptr) available.add(p);
            }
            if (available.size() > 0) {
                Random r;
                int index = r.nextInt(available.size());
                newId = available[index]->id->intValue();
            }
        }
        else {
            newId = useAnotherOtherId->intValue();
        }
        target->useAnotherId->setValue(newId);
    }
    else if (type == "Group") {
        Group* target = Brain::getInstance()->getGroupById(useAnotherTargetId->intValue());
        if (target == nullptr) { return; }
        if (useAnotherRandomize->boolValue()) {
            Array<Group*> available;
            for (int i = min; i <= max; i++) {
                Group* p = Brain::getInstance()->getGroupById(i);
                if (p != nullptr) available.add(p);
            }
            if (available.size() > 0) {
                Random r;
                int index = r.nextInt(available.size());
                newId = available[index]->id->intValue();
            }
        }
        else {
            newId = useAnotherOtherId->intValue();
        }
        target->useAnotherId->setValue(newId);
    }


}

void InputPanelAction::onContainerParameterChangedInternal(Parameter* p)
{
    if (p == useAnotherRandomize) {
        updateDisplay();
    }
}

void InputPanelAction::updateDisplay()
{
    if (actionType == IP_USEANOTHER) {
        bool rand = useAnotherRandomize->boolValue();
        useAnotherOtherId->hideInEditor = rand;
        useAnotherFromId->hideInEditor = !rand;
        useAnotherToId->hideInEditor = !rand;
    }
    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));

}

var InputPanelAction::getValue()
{
    var val = var();

    switch (actionType)
    {
    case IP_PRESS:
        break;

    case IP_GM:
        val = InputPanel::getInstance()->paramGrandMaster->floatValue();
        break;

    case IP_OFFCL:
        break;

    case IP_KILLCL:
        break;

    case IP_STOPFX:
        break;

    case IP_STOPCAR:
        break;

    case IP_RANDOMSEED:
        break;

    }

    return val;
}
