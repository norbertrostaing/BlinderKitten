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

void CuelistAction::setValueInternal(var value, String origin) {
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
        if (val > 0 && (float)previousValue == 0) {
            target->go();
        }
        break;

    case CL_GOBACK:
        if (val > 0 && (float)previousValue == 0) {
            target->goBack();
        }
        break;

    case CL_GORANDOM:
        if (val > 0 && (float)previousValue == 0) {
            target->go();
        }
        break;

    case CL_OFF:
        if (val > 0 && (float)previousValue == 0) {
            target->off();
        }
        break;

    case CL_TOGGLE:
        if (val > 0 && (float)previousValue == 0) {
            if (target->isCuelistOn->getValue()) {
                target->off();
            }
            else {
                target->go();
            }
        }
        break;

    case CL_LOAD:
        if (val > 0 && (float)previousValue == 0) {
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
        if (val > 0 && (float)previousValue == 0) {
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
        if (target->currentHTPLevelController == origin || abs(target->HTPLevel->floatValue() - val) < 0.05) {
            target->nextHTPLevelController = origin;
            target->HTPLevel->setValue(val);
        }
        break;

    case CL_LTPLEVEL:
        if (target->currentLTPLevelController == origin || abs(target->LTPLevel->floatValue() - val) < 0.05) {
            target->nextLTPLevelController = origin;
            target->LTPLevel->setValue(val);
        }
        break;

    case CL_CHASERSPEED:
        target->chaserSpeed->setValue(val*maxSpeed->floatValue());
        break;

    case CL_FLASH:
        if (val > 0 && (float)previousValue == 0) {
            target->flash(true, false);
        }
        else {
            target->flash(false, false);
        }
        break;

    case CL_SWOP:
        if (val > 0 && (float)previousValue == 0) {
            target->flash(true, false, true);
        }
        else {
            target->flash(false, false);
        }
        break;

    case CL_FLASHLEVEL:
        if (target->currentFlashLevelController == origin || abs(target->FlashLevel->floatValue() - val) < 0.05) {
            target->nextFlashLevelController = origin;
            target->FlashLevel->setValue(val);
        }
        break;

    case CL_CHASERTAPTEMPO:
        if (val > 0 && (float)previousValue == 0) {
            target->tapTempo();
        }
        break;


    }


}