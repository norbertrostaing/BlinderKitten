/*
  ==============================================================================

    SelectionMasterAction.cpp
    Created: 3 Feb 2022 10:15:35pm
    Author:  No

  ==============================================================================
*/

#include "SelectionMasterAction.h"
#include "../SelectionMaster/SelectionMaster.h"
#include "../../Brain.h"

SelectionMasterAction::SelectionMasterAction(var params) :
    Action(params)
{
    actionType = (ActionType)(int)params.getProperty("actionType", SELM_START);

    targetId = addIntParameter("SelectionMaster ID", "Id of the target SelectionMaster", 0, 0);

    if (actionType == SELM_STATIC_SIZE) {
        staticSize = addFloatParameter("Size", "", 1);
    }
    else if (actionType == SELM_SIZE) {
        sizeRange = addPoint2DParameter("Size range", "");
        sizeRange->setDefaultPoint(0,1);
    }

}

SelectionMasterAction::~SelectionMasterAction()
{
}

void SelectionMasterAction::triggerInternal()
{
}

void SelectionMasterAction::setValueInternal(var value, String origin, int incrementIndex, bool isRelative) {
    SelectionMaster* target = Brain::getInstance()->getSelectionMasterById(targetId->getValue());
    if (target == nullptr) return;

    float val = value;
    bool incrementOk = incrementIndex == 0 || incrementIndex == validIncrementIndex;

    switch (actionType)
    {
    case SELM_START:
        if (val == 1) {
            target->start();
        }
        break;

    case SELM_STOP:
        if (val == 1) {
            target->stop();
        }
        break;

    case SELM_TOGGLE:
        if (val == 1) {
            if (target->isOn) {
                target->stop();
            }
            else {
                target->start();
            }
        }
        break;

    case SELM_STATIC_SIZE:
        if (val == 1) {
            target->sizeValue->setValue(staticSize->floatValue());
        }

    case SELM_SIZE:
        if (isRelative) {
            target->nextSizeController = origin;
            float newVal = target->sizeValue->floatValue() + val;
            newVal = jlimit(sizeRange->x, sizeRange->y, newVal);
            target->sizeValue->setValue(newVal);
        }
        else {
            float newVal = jmap(val,0.f,1.f, sizeRange->x, sizeRange->y);
            target->sizeValue->setValue(newVal);
        }
        break;

    }
}

var SelectionMasterAction::getValue()
{
    var val = var();

    SelectionMaster* target = Brain::getInstance()->getSelectionMasterById(targetId->getValue());
    if (target == nullptr) return val;

    switch (actionType)
    {
    case SELM_START:
        break;

    case SELM_STOP:
        break;

    case SELM_TOGGLE:
        break;

    case SELM_SIZE:
        val = target->sizeValue->floatValue();
        break;

    }

    return val;
}
