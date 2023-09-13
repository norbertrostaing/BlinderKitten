/*
  ==============================================================================

    StampAction.cpp
    Created: 3 Feb 2022 10:15:35pm
    Author:  No

  ==============================================================================
*/

#include "StampAction.h"
#include "../Stamp/Stamp.h"
#include "../../Brain.h"

StampAction::StampAction(var params) :
    Action(params)
{
    actionType = (ActionType)(int)params.getProperty("actionType", STMP_START);

    targetId = addIntParameter("Stamp ID", "Id oth the target effect", 0, 0);

}

StampAction::~StampAction()
{
}

void StampAction::triggerInternal()
{
}

void StampAction::setValueInternal(var value, String origin, bool isRelative) {
    Stamp* target = Brain::getInstance()->getStampById(targetId->getValue());
    if (target == nullptr) return;

    float val = value;

    switch (actionType)
    {
    case STMP_START:
        if (val > 0 && (float)previousValue == 0) {
            target->start();
        }
        break;

    case STMP_STOP:
        if (val > 0 && (float)previousValue == 0) {
            target->stop();
        }
        break;

    case STMP_TOGGLE:
        if (val > 0 && (float)previousValue == 0) {
            if (target->isOn) {
                target->stop();
            }
            else {
                target->start();
            }
        }
        break;

    case STMP_SIZE:
        if (isRelative) {
            target->nextSizeController = origin;
            target->sizeValue->setValue(target->sizeValue->floatValue() + val);
        }
        else {
            if (origin == "" || target->currentSizeController == origin || abs(target->sizeValue->floatValue() - val) < 0.05) {
                target->nextSizeController = origin;
                target->sizeValue->setValue(val);
            }
        }
        break;
    }
}

var StampAction::getValue()
{
    var val = var();

    Stamp* target = Brain::getInstance()->getStampById(targetId->getValue());
    if (target == nullptr) return val;

    switch (actionType)
    {
    case STMP_START:
        break;

    case STMP_STOP:
        break;

    case STMP_TOGGLE:
        break;

    case STMP_SIZE:
        val = target->sizeValue->floatValue();
        break;
    
    }
    return val;
}
