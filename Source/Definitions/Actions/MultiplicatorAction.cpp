/*
  ==============================================================================

    MultiplicatorAction.cpp
    Created: 3 Feb 2022 10:15:35pm
    Author:  No

  ==============================================================================
*/

#include "MultiplicatorAction.h"
#include "../Multiplicator/MultiplicatorManager.h"
#include "../../Brain.h"

MultiplicatorAction::MultiplicatorAction(var params) :
    Action(params)
{
    actionType = (ActionType)(int)params.getProperty("actionType", MULT_SET);

    targetMult = addTargetParameter("Multiplicator", "Target multiplicator", MultiplicatorManager::getInstance());
    targetMult->targetType = TargetParameter::CONTAINER;
    targetMult->maxDefaultSearchLevel = 0;

    if (actionType == MULT_SET) {
        fromValue = addFloatParameter("Value from", "Value when midi is at 0", 0, 0);
        toValue = addFloatParameter("Value from", "Value when midi is at 127", 1, 0);
    }

    if (actionType == MULT_SETAT) {
        atValue = addFloatParameter("Value", "new value of the multiplicator", 1, 0);
    }
}

MultiplicatorAction::~MultiplicatorAction()
{
}

void MultiplicatorAction::triggerInternal()
{
}

void MultiplicatorAction::setValueInternal(var value, String origin) {
    Multiplicator* target = dynamic_cast<Multiplicator*>(targetMult->targetContainer.get());
    if (target == nullptr) return;

    float val = value;

    switch (actionType)
    {
    case MULT_SETAT:
        if (val > 0) {
            target->multValue->setValue((float)atValue->getValue());
        }
        break;

    case MULT_SET:
        val = jmap(val, (float)fromValue->getValue(), (float)toValue->getValue());
        target->multValue->setValue(val);
        break;
    }
}