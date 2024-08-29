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

    if (actionType == BUN_SIZE) {
        useHTP = addBoolParameter("Move HTP", "Change size of HTP Level of cuelists", true);
        useLTP = addBoolParameter("Move LTP", "Change size of LTP Level of cuelists", false);
        useSize = addBoolParameter("Move Size", "Change size of effects, carousels, mappers and trackers", true);
        useFlash = addBoolParameter("Move Flash", "change flash levels", false);
    }

}

BundleAction::~BundleAction()
{
}

void BundleAction::triggerInternal()
{
}

void BundleAction::setValueInternal(var value, String origin, bool isRelative) {
    Bundle* target = Brain::getInstance()->getBundleById(targetId->getValue());
    if (target == nullptr) return;

    float val = value;

    switch (actionType)
    {
    case BUN_START:
        if (val > 0 && (float)previousValue == 0) {
            target->start();
        }
        break;

    case BUN_STOP:
        if (val > 0 && (float)previousValue == 0) {
            target->stop();
        }
        break;

    case BUN_TAPTEMPO:
        if (val > 0 && (float)previousValue == 0) {
            target->tapTempo();
        }
        break;

    case BUN_SIZE:
        if (isRelative) {
            target->setSizeRel(val, useSize->boolValue(), useHTP->boolValue(), useLTP->boolValue(), useFlash->boolValue());
        }
        else {
            target->setSize(val, useSize->boolValue(), useHTP->boolValue(), useLTP->boolValue(), useFlash->boolValue());
            lastValue = val;
        }
        break;

    case BUN_SPEED:
        if (isRelative) {
            val *= (float)maxSpeed->getValue();
            target->setSpeedRel(val);
        }
        else {
            lastValue = val;
            val *= (float)maxSpeed->getValue();
            target->setSpeed(val);
        }
        break;

    case BUN_DOUBLESPEED:
        if (val > 0 && (float)previousValue == 0) {
            target->speedMult(2);
        }
        break;

    case BUN_HALFSPEED:
        if (val > 0 && (float)previousValue == 0) {
            target->speedMult(0.5);
        }

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
        val = lastValue;
        break;

    case BUN_SPEED:
        val = lastValue;
        break;

    case BUN_DOUBLESPEED:
        break;

    case BUN_HALFSPEED:
        break;
    }

    return val;
}