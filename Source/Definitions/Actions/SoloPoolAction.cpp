/*
  ==============================================================================

    SoloPoolAction.cpp
    Created: 3 Feb 2022 10:15:35pm
    Author:  No

  ==============================================================================
*/

#include "SoloPoolAction.h"
#include "../Bundle/Bundle.h"
#include "../../Brain.h"

SoloPoolAction::SoloPoolAction(var params) :
    Action(params)
{
    actionType = (ActionType)(int)params.getProperty("actionType", SOLP_STOP);

    targetId = addIntParameter("Solo pool ID", "Id of the target solo pool", 1, 1);
    targetIdThru = addBoolParameter("Thru", "", false);
    targetIdTo = addIntParameter("Solo pool ID To", "Id of the last target solo pool", 1, 1);

    if (actionType == SOLP_SPEED) {
        maxSpeed = addFloatParameter("Max Speed", "Speed when your fader is up high", 600, 0);
        adaptSpeedTapTempo = addBoolParameter("Adapt with beat by cycles", "If checked, the speed will be set depending the tap tempo beat by cycle option", false);
    }

    if (actionType == SOLP_SET_SPEED) {
        finalSpeed = addFloatParameter("Speed", "Speed you want to give", 600, 0);
        adaptSpeedTapTempo = addBoolParameter("Adapt with beat by cycles", "If checked, the speed will be set depending the tap tempo beat by cycle option", false);
    }


    if (actionType == SOLP_SIZE || actionType == SOLP_SET_SIZE) {
        useHTP = addBoolParameter("Move HTP", "Change size of HTP Level of cuelists", true);
        useLTP = addBoolParameter("Move LTP", "Change size of LTP Level of cuelists", false);
        useSize = addBoolParameter("Move Size", "Change size of effects, carousels, mappers and trackers", true);
        useFlash = addBoolParameter("Move Flash", "change flash levels", false);
        if (actionType == SOLP_SET_SIZE) {
            finalSize = addFloatParameter("Size", "final size", 0, 0, 1);
        }
    }

    if (actionType == SOLP_STOP) {
        forcedFade = addFloatParameter("Force fade", "Force a fade time", 0, 0);
        forcedFade->setEnabled(false);
        forcedFade->canBeDisabledByUser = true;
    }

    if (actionType == SOLP_FADE) {
        inFade = addBoolParameter("In fade", "For effects and carousels only", true);
        outFade = addBoolParameter("Out fade", "For cuelists, effects and carousels", true);

        fadeDurationFrom = addFloatParameter("Fade duration from", "", 0, 0);
        fadeDurationTo = addFloatParameter("Fade duration to", "", 0, 0);
    }

    if (actionType == SOLP_FADE_FIXED) {
        inFade = addBoolParameter("In fade", "For effects and carousels only", true);
        outFade = addBoolParameter("Out fade", "For cuelists, effects and carousels", true);

        fadeDurationFixed = addFloatParameter("Fade duration", "", 0, 0);
    }

    updateDisplay();
}

SoloPoolAction::~SoloPoolAction()
{
}

void SoloPoolAction::triggerInternal()
{
}

void SoloPoolAction::setValueInternal(var value, String origin, int indexIncrement, bool isRelative) {
    float val = value;
    int from = targetId->intValue();
    int to = targetIdThru->boolValue() ? targetIdTo->intValue() : from;
    if (from > to) {
        int temp = to; to = from; from = temp;
    }

    for (int i = from; i <= to; i++) {

        switch (actionType)
        {
        case SOLP_RANDOM:
            if (val == 1) {
                Brain::getInstance()->soloPoolRandom(i);
            }
            break;

        case SOLP_STOP:
            if (val == 1) {
                Brain::getInstance()->soloPoolStop(i);
            }
            break;
       
       case SOLP_START:
            if (val == 1) {
                Brain::getInstance()->soloPoolStart(i, forcedFade->enabled ? forcedFade->floatValue() : -1);
            }
            break;

        case SOLP_STOP_INSTANT:
            if (val == 1) {
                Brain::getInstance()->soloPoolStop(i,0);
            }
            break;

        case SOLP_TAPTEMPO:
            if (val == 1) {
                Brain::getInstance()->soloPoolTapTempo(i);
            }
            break;

        case SOLP_SIZE:
            if (isRelative) {
                Brain::getInstance()->soloPoolSetSizeRel(i,val, useSize->boolValue(), useHTP->boolValue(), useLTP->boolValue(), useFlash->boolValue());
            }
            else {
                Brain::getInstance()->soloPoolSetSize(i,val, useSize->boolValue(), useHTP->boolValue(), useLTP->boolValue(), useFlash->boolValue());
            }
            break;

        case SOLP_SET_SIZE:
            if (val == 1) {
                Brain::getInstance()->soloPoolSetSize(i,finalSize->floatValue(), useSize->boolValue(), useHTP->boolValue(), useLTP->boolValue(), useFlash->boolValue());
            }
            break;

        case SOLP_SPEED:
            if (isRelative) {
                val *= (float)maxSpeed->getValue();
                Brain::getInstance()->soloPoolSetSpeedRel(i,val, adaptSpeedTapTempo->boolValue());
            }
            else {
                val *= (float)maxSpeed->getValue();
                Brain::getInstance()->soloPoolSetSpeed(i,val, adaptSpeedTapTempo->boolValue());
            }
            break;

        case SOLP_SET_SPEED:
            if (val == 1) {
                Brain::getInstance()->soloPoolSetSpeed(i,finalSpeed->floatValue(), adaptSpeedTapTempo->boolValue());
            }
            break;


        case SOLP_DOUBLESPEED:
            if (val == 1) {
                Brain::getInstance()->soloPoolSpeedMult(i,2);
            }
            break;

        case SOLP_HALFSPEED:
            if (val == 1) {
                Brain::getInstance()->soloPoolSpeedMult(i,0.5);
            }
            break;

        case SOLP_FLASH:
            Brain::getInstance()->soloPoolFlash(i,val == 1, false, false);
            break;

        case SOLP_TIMED_FLASH:
            Brain::getInstance()->soloPoolFlash(i,val == 1, true, false);
            break;

        case SOLP_SWOP:
            Brain::getInstance()->soloPoolFlash(i,val == 1, false, true);
            break;

        case SOLP_TIMED_SWOP:
            Brain::getInstance()->soloPoolFlash(i,val == 1, true, true);
            break;

        case SOLP_FADE:
        {
            float fade = jmap(val, 0.f, 1.f, fadeDurationFrom->floatValue(), fadeDurationTo->floatValue());
            if (inFade->boolValue()) Brain::getInstance()->soloPoolSetFadeIn(i,fade);
            if (outFade->boolValue()) Brain::getInstance()->soloPoolSetFadeOut(i,fade);
            break;
        }

        case SOLP_FADE_FIXED:
            if (inFade->boolValue()) Brain::getInstance()->soloPoolSetFadeIn(i,fadeDurationFixed->floatValue());
            if (outFade->boolValue()) Brain::getInstance()->soloPoolSetFadeOut(i,fadeDurationFixed->floatValue());
            break;

        }

    }

}

void SoloPoolAction::onContainerParameterChangedInternal(Parameter* p)
{
    if (p == targetIdThru) updateDisplay();
}

void SoloPoolAction::updateDisplay()
{
    targetIdTo->hideInEditor = !targetIdThru->boolValue();

    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}


var SoloPoolAction::getValue()
{
    var val = var();

    return val;
}
