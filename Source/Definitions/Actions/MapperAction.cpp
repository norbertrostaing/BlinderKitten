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

void MapperAction::setValueInternal(var value) {
    Mapper* target = Brain::getInstance()->getMapperById(targetId->getValue());
    if (target == nullptr) return;

    float val = value;

    switch (actionType)
    {
    case TRK_START:
        if (val > 0) {
            target->start();
        }
        break;

    case TRK_STOP:
        if (val > 0) {
            target->stop();
        }
        break;

    case TRK_TOGGLE:
        if (val > 0) {
            if (target->isOn) {
                target->stop();
            }
            else {
                target->start();
            }
        }
        break;

    case TRK_SIZE:
        target->sizeValue->setValue(val);
        break;

    }
}