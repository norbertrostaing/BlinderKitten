/*
  ==============================================================================

    MapperAction.cpp
    Created: 3 Feb 2022 10:15:35pm
    Author:  No

  ==============================================================================
*/

#include "MapperAction.h"
#include "../Mapper/Mapper.h"
#include "../../Brain.h"

MapperAction::MapperAction(var params) :
    Action(params)
{
    actionType = (ActionType)(int)params.getProperty("actionType", TRK_START);

    targetId = addIntParameter("Mapper ID", "Id oth the target Mapper", 0, 0);

}

MapperAction::~MapperAction()
{
}

void MapperAction::triggerInternal()
{
}

void MapperAction::setValueInternal(var value, String origin, int incrementIndex, bool isRelative) {
    Mapper* target = Brain::getInstance()->getMapperById(targetId->getValue());
    if (target == nullptr) return;

    float val = value;
    bool incrementOk = incrementIndex == 0 || incrementIndex == validIncrementIndex;

    switch (actionType)
    {
    case TRK_START:
        if (val == 1) {
            target->start();
        }
        break;

    case TRK_STOP:
        if (val == 1) {
            target->stop();
        }
        break;

    case TRK_TOGGLE:
        if (val == 1) {
            if (target->isOn) {
                target->stop();
            }
            else {
                target->start();
            }
        }
        break;

    case TRK_SIZE:
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

    }
}

var MapperAction::getValue()
{
    var val = var();

    Mapper* target = Brain::getInstance()->getMapperById(targetId->getValue());
    if (target == nullptr) return val;

    switch (actionType)
    {
    case TRK_START:
        break;

    case TRK_STOP:
        break;

    case TRK_TOGGLE:
        break;

    case TRK_SIZE:
        val = target->sizeValue->floatValue();
        break;

    }

    return val;
}
