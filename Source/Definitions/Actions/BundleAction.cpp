/*
  ==============================================================================

    BundleAction.cpp
    Created: 3 Feb 2022 10:15:35pm
    Author:  No

  ==============================================================================
*/

#include "BundleAction.h"
#include "../Bundle/Bundle.h"
#include "../../Brain.h"

BundleAction::BundleAction(var params) :
    Action(params)
{
    actionType = (ActionType)(int)params.getProperty("actionType", BUN_START);

    targetId = addIntParameter("Bundle ID", "Id oth the target effect", 0, 0);

    if (actionType == BUN_SPEED) {
        maxSpeed = addFloatParameter("Max Speed", "Speed when your fader is up high", 600, 0);
    }

    if (actionType == BUN_SET_SPEED) {
        finalSpeed = addFloatParameter("Speed", "Speed you want to give", 600, 0);
    }


    if (actionType == BUN_SIZE || actionType == BUN_SET_SIZE) {
        useHTP = addBoolParameter("Move HTP", "Change size of HTP Level of cuelists", true);
        useLTP = addBoolParameter("Move LTP", "Change size of LTP Level of cuelists", false);
        useSize = addBoolParameter("Move Size", "Change size of effects, carousels, mappers and trackers", true);
        useFlash = addBoolParameter("Move Flash", "change flash levels", false);
        if (actionType == BUN_SET_SIZE) {
            finalSize = addFloatParameter("Size", "final size", 0, 0, 1);
        }
    }

}

BundleAction::~BundleAction()
{
}

void BundleAction::triggerInternal()
{
}

void BundleAction::setValueInternal(var value, String origin, int indexIncrement, bool isRelative) {
    Bundle* target = Brain::getInstance()->getBundleById(targetId->getValue());
    if (target == nullptr) return;

    float val = value;

    switch (actionType)
    {
    case BUN_START:
        if (val == 1) {
            target->start();
        }
        break;

    case BUN_STOP:
        if (val == 1) {
            target->stop();
        }
        break;

    case BUN_STOP_INSTANT:
        if (val == 1) {
            target->stop(0);
        }
        break;

    case BUN_TAPTEMPO:
        if (val == 1) {
            target->tapTempo();
        }
        break;

    case BUN_SIZE:
        if (isRelative) {
            target->setSizeRel(val, useSize->boolValue(), useHTP->boolValue(), useLTP->boolValue(), useFlash->boolValue());
        }
        else {
            target->setSize(val, useSize->boolValue(), useHTP->boolValue(), useLTP->boolValue(), useFlash->boolValue());
        }
        break;

    case BUN_SET_SIZE:
        if (val == 1) {
            target->setSize(finalSize->floatValue(), useSize->boolValue(), useHTP->boolValue(), useLTP->boolValue(), useFlash->boolValue());
        }
        break;

    case BUN_SPEED:
        if (isRelative) {
            val *= (float)maxSpeed->getValue();
            target->setSpeedRel(val);
        }
        else {
            val *= (float)maxSpeed->getValue();
            target->setSpeed(val);
        }
        break;

    case BUN_SET_SPEED:
        if (val == 1) {
            target->setSpeed(finalSpeed->floatValue());
        }
        break;


    case BUN_DOUBLESPEED:
        if (val == 1) {
            target->speedMult(2);
        }
        break;

    case BUN_HALFSPEED:
        if (val == 1) {
            target->speedMult(0.5);
        }
        break;

    case BUN_FLASH:
        target->flash(val == 1, false, false);
        break;

    case BUN_TIMED_FLASH:
        target->flash(val == 1, true, false);
        break;

    case BUN_SWOP:
        target->flash(val == 1, false, true);
        break;

    case BUN_TIMED_SWOP:
        target->flash(val == 1, true, true);
        break;

    }

}

var BundleAction::getValue()
{
    var val = var();

    Bundle* target = Brain::getInstance()->getBundleById(targetId->getValue());
    if (target == nullptr) return val;

    switch (actionType)
    {
    case BUN_START:
        break;

    case BUN_STOP:
        break;

    case BUN_TAPTEMPO:
        break;

    case BUN_SIZE:
        if (useHTP->boolValue()) val = target->lastHTP;
        else if (useLTP->boolValue()) val = target->lastLTP;
        else if (useSize->boolValue()) val = target->lastSize;
        else if (useFlash->boolValue()) val = target->lastFlash;
        break;

    case BUN_SPEED:
        val = target->lastSpeed/(jmax(maxSpeed->floatValue(),0.001f));
        break;

    case BUN_DOUBLESPEED:
        break;

    case BUN_HALFSPEED:
        break;
    }

    return val;
}
