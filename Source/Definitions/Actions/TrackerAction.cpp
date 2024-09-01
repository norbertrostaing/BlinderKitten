/*
  ==============================================================================

    TrackerAction.cpp
    Created: 3 Feb 2022 10:15:35pm
    Author:  No

  ==============================================================================
*/

#include "TrackerAction.h"
#include "../Tracker/Tracker.h"
#include "../../Brain.h"

TrackerAction::TrackerAction(var params) :
    Action(params)
{
    actionType = (ActionType)(int)params.getProperty("actionType", TRK_START);

    targetId = addIntParameter("Tracker ID", "Id oth the target Tracker", 0, 0);

}

TrackerAction::~TrackerAction()
{
}

void TrackerAction::triggerInternal()
{
}

void TrackerAction::setValueInternal(var value, String origin, bool isRelative) {
    Tracker* target = Brain::getInstance()->getTrackerById(targetId->getValue());
    if (target == nullptr) return;

    float val = value;

    switch (actionType)
    {
    case TRK_START:
        if (val == 1) {
            target->start();
        }
        break;

    case TRK_STOP:
        if (val == 1) {
            target->stop();
        }
        break;

    case TRK_TOGGLE:
        if (val == 1) {
            if (target->isOn) {
                target->stop();
            }
            else {
                target->start();
            }
        }
        break;

    case TRK_SIZE:
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

    }
}

var TrackerAction::getValue()
{
    var val = var();

    Tracker* target = Brain::getInstance()->getTrackerById(targetId->getValue());
    if (target == nullptr) return val;

    switch (actionType)
    {
    case TRK_START:
        break;

    case TRK_STOP:
        break;

    case TRK_TOGGLE:
        break;

    case TRK_SIZE:
        val = target->sizeValue->floatValue();
        break;

    }

    return val;
}
