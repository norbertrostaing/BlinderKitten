/*
  ==============================================================================

    EncoderAction.cpp
    Created: 3 Feb 2022 10:15:35pm
    Author:  No

  ==============================================================================
*/

#include "EncoderAction.h"
#include "../Carousel/Carousel.h"
#include "../../Brain.h"
#include "UserInputManager.h"
#include "UI/InputPanel.h"
#include "UI/Encoders.h"

EncoderAction::EncoderAction(var params) :
    Action(params)
{
    actionType = (ActionType)(int)params.getProperty("actionType", ENC_VALUE);

    if (actionType == ENC_VALUE) {
        targetEncoder = addIntParameter("Encoder", "Wich encoder do you want to modify ?", 1, 1, 10);
    }
    else if(actionType == ENC_SELECT) {
        selectionDelta = addIntParameter("Selection delta", "If positive, it will select nth next encoder, if negative, nth previous encoder", 1);
    }
    else if(actionType == ENC_TOGGLEFILTER) {
        filterNumber = addIntParameter("Filter", "Wich filter do you want to toggle ?", 1, 1);
    }
}

EncoderAction::~EncoderAction()
{
}

void EncoderAction::triggerInternal()
{
}

void EncoderAction::setValueInternal(var value, String origin, bool isRelative) {
    float val = value;

    switch (actionType)
    {
    case ENC_VALUE:
        int index;
        index = targetEncoder->intValue() - 1;
        if (index >= 0 && index < Encoders::getInstance()->encoders.size()) {
            if (isRelative) {
                float baseValue = Encoders::getInstance()->encoders[index]->getValue();
                UserInputManager::getInstance()->encoderValueChanged(index, baseValue+(float)value);
            }
            else {
                UserInputManager::getInstance()->encoderValueChanged(index, value);
            }
        }
        break;

    case ENC_SELECT:
        if (val > 0) {
            int delta;
            delta = selectionDelta->intValue();
            Encoders::getInstance()->offsetEncoders(delta);
        }
        break;

    case ENC_NEXTCOMMAND:
        if (val > 0) {
            Encoders::getInstance()->commandUpBtn.triggerClick();
        }
        break;

    case ENC_PREVCOMMAND:
        if (val > 0) {
            Encoders::getInstance()->commandDownBtn.triggerClick();
        }
        break;

    case ENC_TOGGLEFILTER:
        if (val > 0) {
            int i;
            i = filterNumber->intValue() - 1;
            if (i < Encoders::getInstance()->filterBtns.size()) {
                Encoders::getInstance()->filterBtns[i]->triggerClick();
            }
        }
        
    break;


    }

}

var EncoderAction::getValue()
{
    var val = var();

    switch (actionType)
    {
    case ENC_VALUE:
        int index;
        index = targetEncoder->intValue() - 1;
        if (index >= 0 && index < Encoders::getInstance()->encoders.size()) {
            val = Encoders::getInstance()->encoders[index]->getValue();
        }
        break;

    case ENC_SELECT:
        break;

    case ENC_NEXTCOMMAND:
        break;

    case ENC_PREVCOMMAND:
        break;

    case ENC_TOGGLEFILTER:
        break;
    }

    return val;
}
