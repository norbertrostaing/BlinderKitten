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
        targetButton->addOption("Edit", "Edit");
        targetButton->addOption("Copy", "Copy");
        targetButton->addOption("Delete", "Delete");
    }
    if (actionType == IP_RANDOMSEED) {
        randomSeed = addIntParameter("Random Seed", "Initialise all random events with this value",0,0);
    }
    if (actionType == IP_SELECTWINDOW) {
        targetWindow = addEnumParameter("Window", "Wich tab do you want to magically appear ?");
        for (int i = 0; i < ShapeShifterFactory::getInstance()->defs.size(); i++) {
            String windowName = ShapeShifterFactory::getInstance()->defs[i]->contentName;
            targetWindow->addOption(windowName, windowName);
        }
    }
}

InputPanelAction::~InputPanelAction()
{
}

void InputPanelAction::triggerInternal()
{
}

void InputPanelAction::setValueInternal(var value, String origin, bool isRelative) {
    float val = value;

    switch (actionType)
    {
    case IP_PRESS:
        if (val > 0) {
            UserInputManager::getInstance()->processInput(targetButton->getValue());
        }
        break;

    case IP_GM:
        {
        const MessageManagerLock mmLock;
        if (isRelative) 
            {
            float v = InputPanel::getInstance()->grandMaster.getValue() + val;
            v = jlimit<float>(0,1,v);
            InputPanel::getInstance()->grandMaster.setValue(v);
            }
        else 
            {
            InputPanel::getInstance()->grandMaster.setValue(val);
            }
        }
        break;

    case IP_OFFCL:
        if (val > 0) {
            Brain::getInstance()->offAllCuelists();
        }
        break;

    case IP_KILLCL:
        if (val > 0) {
            Brain::getInstance()->killAllCuelists();
        }
        break;

    case IP_STOPFX:
        if (val > 0) {
            Brain::getInstance()->stopAllEffects();
        }
        break;

    case IP_STOPCAR:
        if (val > 0) {
            Brain::getInstance()->stopAllCarousels();
        }
        break;

    case IP_RANDOMSEED:
        if (val > 0) {
            Brain::getInstance()->resetRandomSeed(randomSeed->getValue());
        }
        break;

    case IP_SELECTWINDOW:
        if (val > 0) {
            Brain::getInstance()->showWindow(targetWindow->stringValue());
        }
        break;

    }

}

var InputPanelAction::getValue()
{
    var val = var();

    switch (actionType)
    {
    case IP_PRESS:
        break;

    case IP_GM:
        val = InputPanel::getInstance()->grandMaster.getValue();
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
