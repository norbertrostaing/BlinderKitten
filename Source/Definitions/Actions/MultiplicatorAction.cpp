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
        toValue = addFloatParameter("Value to", "Value when midi is at 127", 1, 0);
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

void MultiplicatorAction::setValueInternal(var value, String origin, int incrementIndex, bool isRelative) {
    Multiplicator* target = dynamic_cast<Multiplicator*>(targetMult->targetContainer.get());
    if (target == nullptr) return;

    float val = value;
    bool incrementOk = incrementIndex == 0 || incrementIndex == validIncrementIndex;

    switch (actionType)
    {
    case MULT_SETAT:
        if (val == 1) {
            target->multValue->setValue((float)atValue->getValue());
        }
        break;

    case MULT_SET:

        if (isRelative) {
            target->multValue->setValue(target->multValue->floatValue() + val);
        }
        else {
            if ((incrementOk ) || abs(target->multValue->floatValue() - val) < 0.05) {
                val = jmap(val, (float)fromValue->getValue(), (float)toValue->getValue());
                target->multValue->setValue(val);
                validIncrementIndex = incrementIndex + 1;
            }
        }

        break;
    }
}

var MultiplicatorAction::getValue()
{
    var val = var();

    Multiplicator* target = dynamic_cast<Multiplicator*>(targetMult->targetContainer.get());
    if (target == nullptr) return val;

    switch (actionType)
    {
    case MULT_SETAT:
        break;

    case MULT_SET:
        val = jmap(target->multValue->floatValue(), fromValue->floatValue(), toValue->floatValue(),0.0f,1.0f);
        break;
    }
    return val;
}
