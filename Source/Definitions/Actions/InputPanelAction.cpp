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
        targetButton->addOption("Record", "Record");
        targetButton->addOption("Edit", "Edit");
        targetButton->addOption("Copy", "Copy");
        targetButton->addOption("Delete", "Delete");
    }
}

InputPanelAction::~InputPanelAction()
{
}

void InputPanelAction::triggerInternal()
{
}

void InputPanelAction::setValueInternal(var value) {
    float val = value;

    switch (actionType)
    {
    case IP_PRESS:
        if (val > 0) {
            UserInputManager::getInstance()->processInput(targetButton->getValue());
        }
        break;

    case IP_GM:
        const MessageManagerLock mmLock;
        InputPanel::getInstance()->grandMaster.setValue(val);
        break;

    }

}