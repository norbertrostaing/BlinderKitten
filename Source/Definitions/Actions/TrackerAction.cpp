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

void TrackerAction::setValueInternal(var value) {
    Tracker* target = Brain::getInstance()->getTrackerById(targetId->getValue());
    if (target == nullptr) return;

    float val = value;

    switch (actionType)
    {
    case TRK_START:
        if (val > 0) {
            target->start();
        }
        break;

    case TRK_STOP:
        if (val > 0) {
            target->stop();
        }
        break;

    case TRK_SIZE:
        target->sizeValue->setValue(val);
        break;

    }
}