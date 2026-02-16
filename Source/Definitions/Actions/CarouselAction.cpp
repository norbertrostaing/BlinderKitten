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
        maxSpeed = addFloatParameter("Max Speed", "Speed when your fader is up high", 600, 0);
    }

    if (actionType == CAR_BBW) {
        buddyBlockOrWing = addEnumParameter("Parameter", "Wich parameter do you want to change");
        buddyBlockOrWing->addOption("Buddy", "Buddy")->addOption("Block", "Block")->addOption("Wing", "Wing");
        setOrAdd = addEnumParameter("Action", "Do you want to set or add a value ?");
        setOrAdd->addOption("Set", "Set")->addOption("Add", "Add");
        carRow = addIntParameter("Row", "the number of the row you want to change, 0 means all of them", 0, 0);
        amount = addIntParameter("Amount", "Your value to set or to add", 1);
    }
    if (actionType == CAR_SET_SEEK || actionType == CAR_ADD_SEEK) {
        seekValue = addFloatParameter("Seek", "Desired seek value or seek delta", 0);
    }

    if (actionType == CAR_START || actionType == CAR_STOP) {
        forcedFade = addFloatParameter("Force fade", "Force a fade time", 0, 0);
        forcedFade->setEnabled(false);
        forcedFade->canBeDisabledByUser = true;
    }
}

CarouselAction::~CarouselAction()
{
}

void CarouselAction::triggerInternal()
{
}

void CarouselAction::setValueInternal(var value, String origin, int incrementIndex, bool isRelative) {
    Carousel* target = Brain::getInstance()->getCarouselById(targetId->getValue());
    if (target == nullptr) return;

    float val = value;
    bool incrementOk = incrementIndex == 0 || incrementIndex == validIncrementIndex;

    switch (actionType)
    {
    case CAR_START:
        if (val == 1) {
            float fade = forcedFade->enabled ? forcedFade->floatValue() : -1;
            target->userStart(fade);
        }
        break;

    case CAR_STOP:
        if (val == 1) {
            float fade = forcedFade->enabled ? forcedFade->floatValue() : -1;
            target->stop(fade);
        }
        break;

    case CAR_STOP_INSTANT:
        if (val == 1) {
            target->stop(0);
        }
        break;

    case CAR_TOGGLE:
        if (val == 1) {
            if (target->isOn) {
                target->stop();
            }
            else {
                target->userStart();
            }
        }
        break;

    case CAR_TAPTEMPO:
        if (val == 1) {
            target->tapTempo();
        }
        break;

    case CAR_FLASH:
        target->flash(val == 1);
        break;

    case CAR_SWOP:
        target->flash(val == 1, true);
        break;

    case CAR_SIZE:
        if (isRelative) {
            target->nextSizeController = origin;
            target->sizeValue->setValue(target->sizeValue->floatValue() + val);
        }
        else {
            if ((incrementOk && target->currentSizeController == origin) || abs(target->sizeValue->floatValue() - val) < 0.05) {
                target->nextSizeController = origin;
                target->sizeValue->setValue(val);
                validIncrementIndex = incrementIndex + 1;
            }
        }
        break;

    case CAR_SPEED:
        val *= (float)maxSpeed->getValue();
        if (isRelative) {
            target->speed->setValue(target->speed->floatValue()+ val);
        }
        else {
            target->speed->setValue(val);
        }
        break;

    case CAR_DOUBLESPEED:
        if (val == 1) {
            target->speed->setValue((double)target->speed->getValue() * 2);
        }
        break;

    case CAR_HALFSPEED:
        if (val == 1) {
            target->speed->setValue((double)target->speed->getValue() / 2);
        }
        break;

    case CAR_BBW:
        if (val == 1) {
            bool add = setOrAdd->stringValue() == "Add";
            String type = buddyBlockOrWing->stringValue();
            int rowId = carRow->intValue();
            int offset = amount->intValue();
            //target->isComputing.enter();
            for (int iRow = 0; iRow < target->rows.items.size(); iRow++) {
                if (rowId == 0 || iRow + 1 == rowId) {
                    CarouselRow* row = target->rows.items[iRow];
                    IntParameter* p = nullptr;
                    if (type == "Buddy") { p = row->buddying; }
                    if (type == "Block") { p = row->blocks; }
                    if (type == "Wing") { p = row->wings; }
                    if (p != nullptr) {
                        const MessageManagerLock mmlock;
                        if (add) {
                            p->setValue(p->intValue() + offset);
                        }
                        else {
                            p->setValue(offset);
                        }
                    }
                }
            }
            //target->isComputing.exit();
        }

        break;

    case CAR_SET_SEEK:
        if (val == 1) {
            double v = seekValue->floatValue();
            v = jmax(0.,v);
            target->totalElapsed = v;
        }
        break;

    case CAR_ADD_SEEK:
        if (val == 1) {
            double v = target->totalElapsed + seekValue->floatValue();
            v = jmax(0., v);
            target->totalElapsed += seekValue->floatValue();
        }
        break;

    }
}

var CarouselAction::getValue()
{
    float val = var();

    Carousel* target = Brain::getInstance()->getCarouselById(targetId->getValue());
    if (target == nullptr) return val;

    switch (actionType)
    {
    case CAR_START:
        break;

    case CAR_STOP:
        break;

    case CAR_TOGGLE:
        break;

    case CAR_TAPTEMPO:
        break;

    case CAR_SIZE:
        val = target->sizeValue->floatValue();
        break;

    case CAR_SPEED:
        val = target->speed->floatValue() / maxSpeed->floatValue();
        break;

    case CAR_DOUBLESPEED:
        break;

    case CAR_HALFSPEED:
        break;

    }
    return val;
}
