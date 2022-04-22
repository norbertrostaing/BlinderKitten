/*
  ==============================================================================

    CarouselAction.cpp
    Created: 3 Feb 2022 10:15:35pm
    Author:  No

  ==============================================================================
*/

#include "CarouselAction.h"
#include "../Carousel/Carousel.h"
#include "../../Brain.h"

CarouselAction::CarouselAction(var params) :
    Action(params)
{
    actionType = (ActionType)(int)params.getProperty("actionType", CAR_START);

    targetId = addIntParameter("Carousel ID", "Id oth the target Carousel", 0, 0);

    if (actionType == CAR_SPEED) {
        maxSpeed = addFloatParameter("Max Speed","Max speed of the Carousel",1,1);
    }
}

CarouselAction::~CarouselAction()
{
}

void CarouselAction::triggerInternal()
{
}

void CarouselAction::setValueInternal(var value) {
    Carousel* target = Brain::getInstance()->getCarouselById(targetId->getValue());
    if (target == nullptr) return;

    float val = value;

    switch (actionType)
    {
    case CAR_START:
        if (val > 0) {
            target->start();
        }
        break;

    case CAR_STOP:
        if (val > 0) {
            target->stop();
        }
        break;

    case CAR_TOGGLE:
        if (val > 0) {
            if (target->isOn) {
                target->stop();
            }
            else {
                target->start();
            }
        }
        break;

    case CAR_TAPTEMPO:
        if (val > 0) {
            target->tapTempo();
        }
        break;

    case CAR_SIZE:
        target->sizeValue->setValue(val);
        break;

    case CAR_SPEED:
        val *= (float)maxSpeed->getValue();
        target->speed->setValue(val);
        break;

    case CAR_DOUBLESPEED:
        if (val > 0) {
            target->speed->setValue((double)target->speed->getValue() * 2);
        }
        break;

    case CAR_HALFSPEED:
        if (val > 0) {
            target->speed->setValue((double)target->speed->getValue() / 2);
        }
        break;

    }
}