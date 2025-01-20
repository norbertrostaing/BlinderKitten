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

    switch (actionType)
    {
    case SOLP_RANDOM:
        if (val == 1) {
            for (int i = from; i <= to; i++) {
                Brain::getInstance()->soloPoolRandom(i);
            }
        }
        break;

    case SOLP_STOP:
        if (val == 1) {
            for (int i = from; i <= to; i++) {
                Brain::getInstance()->soloPoolStop(i);
            }
        }
        break;

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
