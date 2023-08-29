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
#include "ChannelFamily/ChannelFamilyManager.h"

EncoderAction::EncoderAction(var params) :
    Action(params)
{
    actionType = (ActionType)(int)params.getProperty("actionType", ENC_VALUE);

    if (actionType == ENC_VALUE) {
        targetEncoder = addIntParameter("Encoder", "Wich encoder do you want to modify ?", 1, 1);
    }
    else if(actionType == ENC_SELECT) {
        selectionDelta = addIntParameter("Selection delta", "If positive, it will select nth next encoder, if negative, nth previous encoder", 1);
    }
    else if (actionType == ENC_TOGGLEFILTERNUM) {
        filterNumber = addIntParameter("Filter", "Wich filter do you want to toggle ?", 1, 1);
    }
    else if (actionType == ENC_TOGGLEFILTERFAMILY) {
        filterFamily = addTargetParameter("Family", "Wich filter do you want to toggle ?", ChannelFamilyManager::getInstance());
        filterFamily->targetType = TargetParameter::CONTAINER;
        filterFamily->maxDefaultSearchLevel = 0;
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
                UserInputManager::getInstance()->encoderValueChanged(index, baseValue + (float)value, "");
            }
            else {
                UserInputManager::getInstance()->encoderValueChanged(index, value, origin);
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

    case ENC_TOGGLEFILTERNUM:
        if (val > 0) {
            int i;
            i = filterNumber->intValue() - 1;
            if (i < Encoders::getInstance()->filterBtns.size()) {
                Encoders::getInstance()->filterBtns[i]->triggerClick();
            }
        }
        
    break;

    case ENC_TOGGLEFILTERFAMILY:
        if (val > 0) {
            ChannelFamily* cf = dynamic_cast<ChannelFamily*>(filterFamily->targetContainer.get());
            if (cf != nullptr) {
                for (int i = 0; i < Encoders::getInstance()->filterBtns.size(); i++) {
                    if (Encoders::getInstance()->filterBtns[i]->getButtonText() == cf->niceName) {
                        Encoders::getInstance()->filterBtns[i]->triggerClick();
                    }
                }
            }
        }
        break;

    case ENC_CLEARFILTERS:
        Encoders::getInstance()->selectedFilters.clear();
        Encoders::getInstance()->updateChannels();
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
            int range = Encoders::getInstance()->encoderRange;
            if (range == 1) { val = (float)val / 100.; }
            if (range == 2) { val = (float)val / 255.; }
        }
        break;

    case ENC_SELECT:
        break;

    case ENC_NEXTCOMMAND:
        break;

    case ENC_PREVCOMMAND:
        break;

    case ENC_TOGGLEFILTERNUM:
        break;
    }

    return val;
}
