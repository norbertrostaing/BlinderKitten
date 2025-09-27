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
        targetEncoder = addIntParameter("Encoder", "Which encoder do you want to modify?", 1, 1);
    }
    else if (actionType == ENC_RESET) {
        targetEncoder = addIntParameter("Encoder", "Which encoder do you want to reset?", 1, 1);
    }
    else if (actionType == ENC_TYPE) {
        encoderType = addTargetParameter("Parameter", "Which encoder parameter do you want to move?", ChannelFamilyManager::getInstance());
        encoderType->targetType = TargetParameter::CONTAINER;
        encoderType->maxDefaultSearchLevel = 2;
        encoderType->typesFilter.add("ChannelType");
    }
    else if (actionType == ENC_SELECT) {
        selectionDelta = addIntParameter("Selection delta", "If positive, it will select nth next encoder, if negative, nth previous encoder", 1);
    }
    else if (actionType == ENC_TOGGLEFILTERNUM) {
        filterNumber = addIntParameter("Filter", "Which filter do you want to toggle ?", 1, 1);
        soloMode = addBoolParameter("Solo", "Disable other filters whene selected",false);
    }
    else if (actionType == ENC_TOGGLEFILTERFAMILY) {
        filterFamily = addTargetParameter("Family", "Which filter do you want to toggle ?", ChannelFamilyManager::getInstance());
        filterFamily->targetType = TargetParameter::CONTAINER;
        filterFamily->maxDefaultSearchLevel = 0;
        filterFamily->typesFilter.add("ChannelFamily");
        soloMode = addBoolParameter("Solo", "Disable other filters when selected", false);
    }
}

EncoderAction::~EncoderAction()
{
}

void EncoderAction::triggerInternal()
{
}

void EncoderAction::setValueInternal(var value, String origin, int incrementIndex, bool isRelative) {
    float val = value;

    switch (actionType)
    {
    case ENC_VALUE:
        int index;
        index = targetEncoder->intValue() - 1;
        if (index >= 0 && index < Encoders::getInstance()->encoders.size()) {
            if (isRelative) {
                float baseValue = Encoders::getInstance()->encoders[index]->getValue();
                if (Encoders::getInstance()->encoderRange == 1) baseValue /= 100.;
                if (Encoders::getInstance()->encoderRange == 2) baseValue /= 255.;

                UserInputManager::getInstance()->encoderValueChanged(index, baseValue + (float)value, "");
            }
            else {
                UserInputManager::getInstance()->encoderValueChanged(index, value, origin);
            }
        }
        break;

    case ENC_RESET:
        index = targetEncoder->intValue() - 1;
        if (index >= 0 && index < Encoders::getInstance()->encoders.size()) {
            UserInputManager::getInstance()->encoderValueChanged(index, -2, origin);
        }
        break;

    case ENC_TYPE:
        {
            ChannelType* ct = dynamic_cast<ChannelType*>(encoderType->targetContainer.get());

            if (ct != nullptr) {
                if (isRelative) {
                    Command* currentCommand = UserInputManager::getInstance()->targetCommand;
                    int i = Encoders::getInstance()->channels.indexOf(ct);
                    if (i>=0 && currentCommand!= nullptr) {
                        float baseValue = currentCommand->getChannelValue(ct);

                        UserInputManager::getInstance()->changeChannelValue(ct, baseValue + (float)value);
                    }
                }
                else {
                    UserInputManager::getInstance()->changeChannelValue(ct, value);
                }

            }
        }
        break;

    case ENC_SELECT:
        if (val == 1) {
            int delta;
            delta = selectionDelta->intValue();
            Encoders::getInstance()->offsetEncoders(delta);
        }
        break;

    case ENC_NEXTCOMMAND:
        if (val == 1) {
            Encoders::getInstance()->paramCommandUp->trigger();
        }
        break;

    case ENC_PREVCOMMAND:
        if (val == 1) {
            Encoders::getInstance()->paramCommandDown->trigger();
        }
        break;

    case ENC_TOGGLEFILTERNUM:
        if (val == 1) {
            int i = filterNumber->intValue() - 1;
            if (i < Encoders::getInstance()->availableFilters.size()) {
                Encoders::getInstance()->toggleFilter(Encoders::getInstance()->availableFilters[i], soloMode->boolValue());
            }
            //if (i < Encoders::getInstance()->filterBtns.size()) {
            //    Encoders::getInstance()->filterBtns[i]->triggerClick();
            //}
        }
        
    break;

    case ENC_TOGGLEFILTERFAMILY:
        if (val == 1) {
            ChannelFamily* cf = dynamic_cast<ChannelFamily*>(filterFamily->targetContainer.get());
            Encoders::getInstance()->toggleFilter(cf, soloMode->boolValue());
        }
        break;

    case ENC_CLEARFILTERS:
        Encoders::getInstance()->selectedFilters.clear();
        Encoders::getInstance()->updateChannels();
        break;

    case ENC_HL:
        if (val == 1) {
            Encoders::getInstance()->paramHighLight->setValue(!Encoders::getInstance()->paramHighLight->boolValue());
        }
        break;
    case ENC_BLIND:
        if (val == 1) {
            Encoders::getInstance()->paramBlind->setValue(!Encoders::getInstance()->paramBlind->boolValue());
        }
        break;
    case ENC_RANGE:
        if (val == 1) {
            Encoders::getInstance()->paramEncoderRange->trigger();
        }
        break;
    case ENC_THRU:
        if (val == 1) {
            Encoders::getInstance()->paramMode->trigger();
        }
        break;
    case ENC_EXPLODE:
        if (val == 1) {
            Encoders::getInstance()->paramExplodeCommand->trigger();
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
