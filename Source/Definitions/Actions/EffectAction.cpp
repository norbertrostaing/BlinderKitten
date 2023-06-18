/*
  ==============================================================================

    EffectAction.cpp
    Created: 3 Feb 2022 10:15:35pm
    Author:  No

  ==============================================================================
*/

#include "EffectAction.h"
#include "../Effect/Effect.h"
#include "../../Brain.h"

EffectAction::EffectAction(var params) :
    Action(params)
{
    actionType = (ActionType)(int)params.getProperty("actionType", FX_START);

    targetId = addIntParameter("Effect ID", "Id oth the target effect", 0, 0);

    if (actionType == FX_SPEED) {
        maxSpeed = addFloatParameter("Max Speed", "Speed when your fader is up high", 600, 0);
    }
}

EffectAction::~EffectAction()
{
}

void EffectAction::triggerInternal()
{
}

void EffectAction::setValueInternal(var value, String origin, bool isRelative) {
    Effect* target = Brain::getInstance()->getEffectById(targetId->getValue());
    if (target == nullptr) return;

    float val = value;

    switch (actionType)
    {
    case FX_START:
        if (val > 0 && (float)previousValue == 0) {
            target->start();
        }
        break;

    case FX_STOP:
        if (val > 0 && (float)previousValue == 0) {
            target->stop();
        }
        break;

    case FX_TOGGLE:
        if (val > 0 && (float)previousValue == 0) {
            if (target->isOn) {
                target->stop();
            }
            else {
                target->start();
            }
        }
        break;

    case FX_TAPTEMPO:
        if (val > 0 && (float)previousValue == 0) {
            target->tapTempo();
        }
        break;

    case FX_SIZE:
        if (isRelative) {
            target->nextSizeController = origin;
            target->sizeValue->setValue(target->sizeValue->floatValue() + val);
        }
        else {
            if (target->currentSizeController == origin || abs(target->sizeValue->floatValue() - val) < 0.05) {
                target->nextSizeController = origin;
                target->sizeValue->setValue(val);
            }
        }
        break;

    case FX_SPEED:
        val *= (float)maxSpeed->getValue();
        if (isRelative) {
            target->speed->setValue(target->speed->floatValue() + val);
        }
        else {
            target->speed->setValue(val);
        }
        break;

    case FX_DOUBLESPEED:
        if (val > 0 && (float)previousValue == 0) {
            target->speed->setValue((double)target->speed->getValue()*2);
        }
        break;

    case FX_HALFSPEED:
        if (val > 0 && (float)previousValue == 0) {
            target->speed->setValue((double)target->speed->getValue()/2);
        }
        break;
    }
}

var EffectAction::getValue()
{
    var val = var();

    Effect* target = Brain::getInstance()->getEffectById(targetId->getValue());
    if (target == nullptr) return val;

    switch (actionType)
    {
    case FX_START:
        break;

    case FX_STOP:
        break;

    case FX_TOGGLE:
        break;

    case FX_TAPTEMPO:
        break;

    case FX_SIZE:
        val = target->sizeValue->floatValue();
        break;

    case FX_SPEED:
        val = target->speed->floatValue() / maxSpeed->floatValue();
        break;

    case FX_DOUBLESPEED:
        break;

    case FX_HALFSPEED:
        break;
    }


    return val;
}
