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
        maxSpeed = addFloatParameter("Max Speed","Max speed of the effect",1,1);
    }
}

EffectAction::~EffectAction()
{
}

void EffectAction::triggerInternal()
{
}

void EffectAction::setValueInternal(var value) {
    Effect* target = Brain::getInstance()->getEffectById(targetId->getValue());
    if (target == nullptr) return;

    float val = value;

    switch (actionType)
    {
    case FX_START:
        if (val > 0) {
            target->start();
        }
        break;

    case FX_STOP:
        if (val > 0) {
            target->stop();
        }
        break;

    case FX_TOGGLE:
        if (val > 0) {
            if (target->isOn) {
                target->stop();
            }
            else {
                target->start();
            }
        }
        break;

    case FX_TAPTEMPO:
        if (val > 0) {
            target->tapTempo();
        }
        break;

    case FX_SIZE:
        target->sizeValue->setValue(val);
        break;

    case FX_SPEED:
        val *= (float)maxSpeed->getValue();
        target->speed->setValue(val);
        break;
    }
}