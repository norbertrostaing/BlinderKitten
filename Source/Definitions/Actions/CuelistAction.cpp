/*
  ==============================================================================

    CuelistAction.cpp
    Created: 3 Feb 2022 10:15:35pm
    Author:  No

  ==============================================================================
*/

#include "CuelistAction.h"
#include "../Cuelist/Cuelist.h"
#include "../../Brain.h"

CuelistAction::CuelistAction(var params) :
    Action(params)
{
    actionType = (ActionType)(int)params.getProperty("actionType", CL_GO);

    if (actionType != CL_GOALLLOADED) {
        cuelistId = addIntParameter("Cuelist ID", "Id oth the target cuelist", 0, 0);
    }
    if (actionType == CL_LOAD || actionType == CL_LOADANDGO) {
        cueId = addFloatParameter("Cue ID", "Insert here the id of the cue you want to load, -1 will prompt the cue choose window", -1, -1);
    }
    if (actionType == CL_CHASERSPEED) {
        maxSpeed = addFloatParameter("Max Speed", "Speed when your fader is up high", 600, 0);
    }
}

CuelistAction::~CuelistAction()
{
}

void CuelistAction::triggerInternal()
{
    Cuelist * target = Brain::getInstance()->getCuelistById(cuelistId->getValue());
    if (target == nullptr) return;
}

void CuelistAction::setValueInternal(var value, String origin, bool isRelative) {
    if (actionType == CL_GOALLLOADED) {
        Brain::getInstance()->goAllLoadedCuelists();
        return;
    }
    Cuelist* target = Brain::getInstance()->getCuelistById(cuelistId->getValue());
    if (target == nullptr) return;

    float val = value;

    switch (actionType)
    {
    case CL_GO:
        if (val == 1) {
            target->go();
        }
        break;

    case CL_GOBACK:
        if (val == 1) {
            target->goBack();
        }
        break;

    case CL_GOINSTANT:
        if (val == 1) {
            target->go(0,0);
        }
        break;

    case CL_GOBACKINSTANT:
        if (val == 1) {
            target->goBack(0,0);
        }
        break;

    case CL_GORANDOM:
        if (val == 1) {
            target->go();
        }
        break;

    case CL_OFF:
        if (val == 1) {
            target->off();
        }
        break;

    case CL_TOGGLE:
        if (val == 1) {
            if (target->isCuelistOn->getValue()) {
                target->off();
            }
            else {
                target->go();
            }
        }
        break;

    case CL_LOAD:
        if (val == 1) {
            float targetCue = cueId->floatValue();
            if (targetCue == -1) {
                target->showLoad();
            }
            else {
                target->nextCueId->setValue(targetCue);
            }
        }
        break;

    case CL_LOADANDGO:
        if (val == 1) {
            float targetCue = cueId->floatValue();
            if (targetCue == -1) {
                target->showLoadAndGo();
            }
            else {
                target->nextCueId->setValue(targetCue);
                target->userGo();
            }
        }
        break;

    case CL_HTPLEVEL:
        if (isRelative) {
            target->nextHTPLevelController = origin;
            target->HTPLevel->setValue(target->HTPLevel->floatValue() + val);
        }
        else {
            if (target->currentHTPLevelController == origin || abs(target->HTPLevel->floatValue() - val) < 0.05) {
                target->nextHTPLevelController = origin;
                target->HTPLevel->setValue(val);
            }
        }
        break;

    case CL_LTPLEVEL:
        if (isRelative) {
            target->nextLTPLevelController = origin;
            target->LTPLevel->setValue(target->LTPLevel->floatValue() + val);
        }
        else {
            if (target->currentLTPLevelController == origin || abs(target->LTPLevel->floatValue() - val) < 0.05) {
            target->nextLTPLevelController = origin;
            target->LTPLevel->setValue(val);
            }
        }
        break;

    case CL_CHASERSPEED:
        if (isRelative) {
            target->chaserSpeed->setValue(target->chaserSpeed->floatValue() + (val * maxSpeed->floatValue()));
        }
        else {
            target->chaserSpeed->setValue(val* maxSpeed->floatValue());
        }
        break;

    case CL_FLASH:
        if (val == 1) {
            target->flash(true, false);
        }
        else {
            target->flash(false, false);
        }
        break;

    case CL_SWOP:
        if (val == 1) {
            target->flash(true, false, true);
        }
        else {
            target->flash(false, false);
        }
        break;

    case CL_FLASHTIMED:
        if (val == 1) {
            target->flash(true, true);
        }
        else {
            target->flash(false, true);
        }
        break;

    case CL_SWOPTIMED:
        if (val == 1) {
            target->flash(true, true, true);
        }
        else {
            target->flash(false, true);
        }
        break;

    case CL_FLASHLEVEL:
        if (isRelative) {
            target->nextFlashLevelController = origin;
            target->FlashLevel->setValue(target->FlashLevel->floatValue() + val);
        }
        else {
            if (target->currentFlashLevelController == origin || abs(target->FlashLevel->floatValue() - val) < 0.05) {
                target->nextFlashLevelController = origin;
                target->FlashLevel->setValue(val);
            }
        }
        break;

    case CL_CHASERTAPTEMPO:
        if (val == 1) {
            target->tapTempo();
        }
        break;

    case CL_CROSSFADE:
        if (isRelative) {
            target->nextCrossFadeController = origin;
            target->crossFadeController->setValue(target->crossFadeController->floatValue() + val);
        }
        else {
            if (target->currentCrossFadeController == origin || abs(target->crossFadeController->floatValue() - val) < 0.05) {
                target->nextCrossFadeController = origin;
                target->crossFadeController->setValue(val);
            }
        }
        break;

    case CL_UPFADE:
        if (isRelative) {
            target->nextUpFadeController = origin;
            target->upFadeController->setValue(target->upFadeController->floatValue() + val);
        }
        else {
            if (target->currentUpFadeController == origin || abs(target->upFadeController->floatValue() - val) < 0.05) {
                target->nextUpFadeController = origin;
                target->upFadeController->setValue(val);
            }
        }
        break;

    case CL_DOWNFADE:
        if (isRelative) {
            target->nextDownFadeController = origin;
            target->downFadeController->setValue(target->downFadeController->floatValue() + val);
        }
        else {
            if (target->currentDownFadeController == origin || abs(target->downFadeController->floatValue() - val) < 0.05) {
                target->nextCrossFadeController = origin;
                target->downFadeController->setValue(val);
            }
        }
        break;


    }


}

var CuelistAction::getValue()
{
    float val = var();

    Cuelist* target = Brain::getInstance()->getCuelistById(cuelistId->getValue());
    if (target == nullptr) return val;

    switch (actionType)
    {
    case CL_GO:
        break;

    case CL_GOBACK:
        break;

    case CL_GOINSTANT:
        break;

    case CL_GOBACKINSTANT:
        break;

    case CL_GORANDOM:
        break;

    case CL_OFF:
        break;

    case CL_TOGGLE:
        break;

    case CL_LOAD:
        break;

    case CL_LOADANDGO:
        break;

    case CL_HTPLEVEL:
        val = target->HTPLevel->floatValue();
        break;

    case CL_LTPLEVEL:
        val = target->LTPLevel->floatValue();
        break;

    case CL_CHASERSPEED:
        val = target->chaserSpeed->floatValue() / maxSpeed->floatValue();
        break;

    case CL_FLASH:
        break;

    case CL_SWOP:
        break;

    case CL_FLASHLEVEL:
        target->FlashLevel->floatValue();
        break;

    case CL_CHASERTAPTEMPO:
        break;
    }

    return val;
}
