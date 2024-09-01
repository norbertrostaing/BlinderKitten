/*
  ==============================================================================

    EffectAction.cpp
    Created: 3 Feb 2022 10:15:35pm
    Author:  No

  ==============================================================================
*/

#include "EffectAction.h"
#include "../Effect/Effect.h"
#include "../../Brain.h"

EffectAction::EffectAction(var params) :
    Action(params)
{
    actionType = (ActionType)(int)params.getProperty("actionType", FX_START);

    targetId = addIntParameter("Effect ID", "Id oth the target effect", 0, 0);

    if (actionType == FX_SPEED) {
        maxSpeed = addFloatParameter("Max Speed", "Speed when your fader is up high", 600, 0);
    }

    if (actionType == FX_BBW) {
        buddyBlockOrWing = addEnumParameter("Parameter", "Wich parameter do you want to change");
        buddyBlockOrWing->addOption("Buddy", "Buddy") ->addOption("Block", "Block") ->addOption("Wing", "Wing");
        setOrAdd = addEnumParameter("Action", "Do you want to set or add a value ?");
        setOrAdd->addOption("Set", "Set")->addOption("Add", "Add");
        fxRow = addIntParameter("Row", "the number of the row you want to change, 0 means all of them", 0, 0);
        fxParam = addIntParameter("Param", "the number of the parameter you want to change, 0 means all of them", 0, 0);
        amount = addIntParameter("Amount", "Your value to set or to add",1);
    }
}

EffectAction::~EffectAction()
{
}

void EffectAction::triggerInternal()
{
}

void EffectAction::setValueInternal(var value, String origin, bool isRelative) {
    Effect* target = Brain::getInstance()->getEffectById(targetId->getValue());
    if (target == nullptr) return;

    float val = value;

    switch (actionType)
    {
    case FX_START:
        if (val == 1) {
            target->userStart();
        }
        break;

    case FX_STOP:
        if (val == 1) {
            target->stop();
        }
        break;

    case FX_TOGGLE:
        if (val == 1) {
            if (target->isOn) {
                target->stop();
            }
            else {
                target->userStart();
            }
        }
        break;

    case FX_FLASH:
        target->flash(val == 1);
        break;

    case FX_SWOP:
        target->flash(val == 1, true);
        break;

    case FX_TAPTEMPO:
        if (val == 1) {
            target->tapTempo();
        }
        break;

    case FX_SIZE:
        if (isRelative) {
            target->nextSizeController = origin;
            target->sizeValue->setValue(target->sizeValue->floatValue() + val);
        }
        else {
            if (target->currentSizeController == origin || abs(target->sizeValue->floatValue() - val) < 0.05) {
                target->nextSizeController = origin;
                target->sizeValue->setValue(val);
            }
        }
        break;

    case FX_SPEED:
        val *= (float)maxSpeed->getValue();
        if (isRelative) {
            target->speed->setValue(target->speed->floatValue() + val);
        }
        else {
            target->speed->setValue(val);
        }
        break;

    case FX_DOUBLESPEED:
        if (val == 1) {
            target->speed->setValue((double)target->speed->getValue()*2);
        }
        break;

    case FX_HALFSPEED:
        if (val == 1) {
            target->speed->setValue((double)target->speed->getValue()/2);
        }
        break;

    case FX_BBW:
        if (val == 1) {
            bool add = setOrAdd->stringValue() == "Add";
            String type = buddyBlockOrWing->stringValue();
            int rowId = fxRow->intValue();
            int paramId = fxParam->intValue();
            int offset = amount->intValue();
            //target->isComputing.enter();
            for (int iRow = 0; iRow < target->values.items.size(); iRow++) {
                if (rowId == 0 || iRow + 1 == rowId) {
                    EffectRow* row = target->values.items[iRow];
                    for (int iParam = 0; iParam < row->paramContainer.items.size(); iParam++) {
                        if (paramId == 0 || iParam + 1 == paramId) {
                            IntParameter* p = nullptr;
                            if (type == "Buddy") { p = row->paramContainer.items[iParam]->buddying; }
                            if (type == "Block") { p = row->paramContainer.items[iParam]->blocks; }
                            if (type == "Wing") { p = row->paramContainer.items[iParam]->wings; }
                            if (p != nullptr) {
                                const MessageManagerLock mmlock;
                                if (add) {
                                    p->setValue(p->intValue()+offset);
                                }
                                else {
                                    p->setValue(offset);
                                }
                            }
                        }
                    }
                }
            }
            //target->isComputing.exit();
        }

        break;
    }
}

var EffectAction::getValue()
{
    var val = var();

    Effect* target = Brain::getInstance()->getEffectById(targetId->getValue());
    if (target == nullptr) return val;

    switch (actionType)
    {
    case FX_START:
        break;

    case FX_STOP:
        break;

    case FX_TOGGLE:
        break;

    case FX_TAPTEMPO:
        break;

    case FX_SIZE:
        val = target->sizeValue->floatValue();
        break;

    case FX_SPEED:
        val = target->speed->floatValue() / maxSpeed->floatValue();
        break;

    case FX_DOUBLESPEED:
        break;

    case FX_HALFSPEED:
        break;
    }


    return val;
}
