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
    actionType = (ActionType)(int)params.getProperty("actionType", ST_START);

    targetId = addIntParameter("Stamp ID", "Id oth the target Stamp", 0, 0);

    if (actionType == ST_START || actionType == ST_STOP) {
        forcedFade = addFloatParameter("Force fade", "Force a fade time", 0,0);
        forcedFade->setEnabled(false);
        forcedFade->canBeDisabledByUser = true;
    }
}

StampAction::~StampAction()
{
}

void StampAction::triggerInternal()
{
}

void StampAction::setValueInternal(var value, String origin, int incrementIndex, bool isRelative) {
    Stamp* target = Brain::getInstance()->getStampById(targetId->getValue());
    if (target == nullptr) return;

    float val = value;
    bool incrementOk = incrementIndex == 0 || incrementIndex == validIncrementIndex;

    switch (actionType)
    {
    case ST_START:
        if (val == 1) {
            float fade = forcedFade->enabled ? forcedFade->floatValue() : -1;
            target->userStart(fade);
        }
        break;

    case ST_STOP:
        if (val == 1) {
            float fade = forcedFade->enabled ? forcedFade->floatValue() : -1;
            target->stop(fade);
        }
        break;

    case ST_STOP_INSTANT:
        if (val == 1) {
            target->stop(0);
        }
        break;

    case ST_TOGGLE:
        if (val == 1) {
            if (target->isOn) {
                target->stop();
            }
            else {
                target->userStart();
            }
        }
        break;

    case ST_FLASH:
        target->flash(val == 1);
        break;

    case ST_SWOP:
        target->flash(val == 1, true);
        break;

    case ST_SIZE:
        if (isRelative) {
            target->nextSizeController = origin;
            target->sizeValue->setValue(target->sizeValue->floatValue() + val);
        }
        else {
            if ((incrementOk && target->currentSizeController == origin) || abs(target->sizeValue->floatValue() - val) < 0.05) {
                target->nextSizeController = origin;
                target->sizeValue->setValue(val);
                validIncrementIndex = incrementIndex + 1;
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
    case ST_START:
        break;

    case ST_STOP:
        break;

    case ST_TOGGLE:
        break;

    case ST_SIZE:
        val = target->sizeValue->floatValue();
        break;

    }


    return val;
}
