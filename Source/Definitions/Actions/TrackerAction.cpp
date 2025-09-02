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

    if (actionType == TRK_AXIS) {
        axis = addEnumParameter("Axis", "Wich axis dou you want to move");
        axis->addOption("X", Xaxis)->addOption("Y", Yaxis)->addOption("Z", Zaxis);

        axisRange = addPoint2DParameter("Range", "Range of the fader");
    }

}

TrackerAction::~TrackerAction()
{
}

void TrackerAction::triggerInternal()
{
}

void TrackerAction::setValueInternal(var value, String origin, int incrementIndex, bool isRelative) {
    Tracker* target = Brain::getInstance()->getTrackerById(targetId->getValue());
    if (target == nullptr) return;

    float val = value;
    bool incrementOk = incrementIndex == 0 || incrementIndex == validIncrementIndex;

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
            if ((incrementOk && target->currentSizeController == origin) || abs(target->sizeValue->floatValue() - val) < 0.05) {
                target->nextSizeController = origin;
                target->sizeValue->setValue(val);
                validIncrementIndex = incrementIndex+1;
            }
        }
        break;

    case TRK_AXIS:
        {
        var vect = target->targetPosition->getValue();
        float vals[3];
        for (int i = 0; i <3; i++) vals[i] = vect[i];
        Axis a = axis->getValueDataAsEnum<Axis>();
        int index = 0;
        if (a == Yaxis) index = 1;
        if (a == Zaxis) index = 2;

        float currentVal = vals[index];
        float newVal = jmap(val, 0.f,1.f, axisRange->x, axisRange->y);
        newVal += isRelative ? currentVal : 0;
        vals[index] = newVal;
        target->targetPosition->setVector(vals[0], vals[1], vals[2]);
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

    case TRK_AXIS:
        {
        var vect = target->targetPosition->getValue();
        Axis a = axis->getValueDataAsEnum<Axis>();
        int index = 0;
        if (a == Yaxis) index = 1;
        if (a == Zaxis) index = 2;

        float currentVal = vect[index];
        val = jmap(currentVal, axisRange->x, axisRange->y, 0.f,1.f);
    }

    }

    return val;
}
