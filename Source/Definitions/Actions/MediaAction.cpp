/*
  ==============================================================================

    MediaAction.cpp
    Created: 3 Feb 2022 10:15:35pm
    Author:  No

  ==============================================================================
*/

#include "MediaAction.h"
#include "../Media/Media.h"
#include "../Media/medias/BKVideo/BKVideo.h"
#include "../../Brain.h"

MediaAction::MediaAction(var params) :
    Action(params)
{
    actionType = (ActionType)(int)params.getProperty("actionType", MED_START);

    targetId = addIntParameter("Media ID", "Id oth the target effect", 0, 0);

    if (actionType == MED_RATE) {
        maxSpeed = addFloatParameter("Max Rate", "Speed when your fader is up high", 10, 0);
    }

}

MediaAction::~MediaAction()
{
}

void MediaAction::triggerInternal()
{
}

void MediaAction::setValueInternal(var value, String origin, bool isRelative) {

    Media* target = Brain::getInstance()->getMediaById(targetId->getValue());
    if (target == nullptr) return;
    BKVideo* videoTarget = dynamic_cast<BKVideo*>(target);
    if (videoTarget == nullptr) return;

    float val = value;
    
    switch (actionType)
    {
    case MED_START:
        if (val > 0 && (float)previousValue == 0) {
            videoTarget->play();
        }
        break;

    case MED_STOP:
        if (val > 0 && (float)previousValue == 0) {
            videoTarget->stop();
        }
        break;

    case MED_PAUSE:
        if (val > 0 && (float)previousValue == 0) {
            videoTarget->pause();
        }
        break;

    case MED_RESTART:
        if (val > 0 && (float)previousValue == 0) {
            videoTarget->restart();
        }
        break;

    case MED_TAPTEMPO:
        if (val > 0 && (float)previousValue == 0) {
            videoTarget->tapTempo();
        }
        break;

    case MED_VOL:
        if (isRelative) {
            videoTarget->nextVolumeController = origin;
            videoTarget->mediaVolume->setValue(videoTarget->mediaVolume->floatValue() + val);
        }
        else {
            if (videoTarget->currentVolumeController == origin || abs(videoTarget->mediaVolume->floatValue() - val) < 0.05) {
                videoTarget->nextVolumeController = origin;
                videoTarget->mediaVolume->setValue(val);
            }
        }
        break;

    case MED_RATE:
        val *= (float)maxSpeed->getValue();
        if (isRelative) {
            videoTarget->speedRate->setValue(videoTarget->speedRate->floatValue() + val);
        }
        else {
            videoTarget->speedRate->setValue(val);
        }
        break;

    case MED_DOUBLESPEED:
        if (val > 0 && (float)previousValue == 0) {
            videoTarget->speedRate->setValue((double)videoTarget->speedRate->getValue()*2);
        }
        break;

    case MED_HALFSPEED:
        if (val > 0 && (float)previousValue == 0) {
            videoTarget->speedRate->setValue((double)videoTarget->speedRate->getValue()/2);
        }
        break;

    }
}

var MediaAction::getValue()
{
    var val = var();

    Media* target = Brain::getInstance()->getMediaById(targetId->getValue());
    if (target == nullptr) return val;
    BKVideo* videoTarget = dynamic_cast<BKVideo*>(target);
    if (videoTarget == nullptr) return val;

    switch (actionType)
    {
    case MED_START:
        break;

    case MED_STOP:
        break;

    case MED_TAPTEMPO:
        break;

    case MED_VOL:
        val = videoTarget->mediaVolume->floatValue();
        break;

    case MED_RATE:
        val = videoTarget->speedRate->floatValue() / maxSpeed->floatValue();
        break;

    case MED_DOUBLESPEED:
        break;

    case MED_HALFSPEED:
        break;
    }


    return val;
}
