/*
  ==============================================================================

    PresetAction.cpp
    Created: 3 Feb 2022 10:15:35pm
    Author:  No

  ==============================================================================
*/

#include "PresetAction.h"
#include "../Bundle/Bundle.h"
#include "../../Brain.h"

PresetAction::PresetAction(var params) :
    Action(params)
{
    actionType = (ActionType)(int)params.getProperty("actionType", PRESET_SET);

    targetId = addIntParameter("Preset id", "it of the new target", 0, 0);
    randomize = addBoolParameter("Randomize", "Random selection of other preset", false);

    otherId = addIntParameter("Other Id", "", 0, 0);
    fromId = addIntParameter("From Id", "", 0, 0);
    toId = addIntParameter("To Id", "", 0, 0);
    updateDisplay();
}

PresetAction::~PresetAction()
{
}

void PresetAction::onContainerParameterChangedInternal(Parameter* p)
{
    if (p == randomize) {
        updateDisplay();
    }
}

void PresetAction::updateDisplay()
{
    bool rand = randomize->boolValue();
    otherId->hideInEditor = rand;
    fromId->hideInEditor = !rand;
    toId->hideInEditor = !rand;
    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}


void PresetAction::triggerInternal()
{
}

void PresetAction::setValueInternal(var value, String origin, int indexIncrement, bool isRelative) {
    float val = value;
    if (val < 1) {
        return;
    }

    int min = fromId->intValue();
    int max = toId->intValue();
    if (min > max) {
        min = toId->intValue();
        max = fromId->intValue();
    }
    int newId = 0;

    if (actionType == PRESET_SET) {
        Preset* target = Brain::getInstance()->getPresetById(targetId->intValue());
        if (target == nullptr) { return; }
        if (randomize->boolValue()) {
            Array<Preset*> available;
            for (int i = min; i <= max; i++) {
                Preset* p = Brain::getInstance()->getPresetById(i);
                if (p != nullptr) available.add(p);
            }
            if (available.size() > 0) {
                Random r;
                int index = r.nextInt(available.size());
                newId = available[index]->id->intValue();
            }
        }
        else {
            newId = otherId->intValue();
        }
        target->useAnotherId->setValue(newId);
    }
    else if (actionType == TIMING_PRESET_SET) {
        TimingPreset* target = Brain::getInstance()->getTimingPresetById(targetId->intValue());
        if (target == nullptr) { return; }
        if (randomize->boolValue()) {
            Array<TimingPreset*> available;
            for (int i = min; i <= max; i++) {
                TimingPreset* p = Brain::getInstance()->getTimingPresetById(i);
                if (p != nullptr) available.add(p);
            }
            if (available.size() > 0) {
                Random r;
                int index = r.nextInt(available.size());
                newId = available[index]->id->intValue();
            }
        }
        else {
            newId = otherId->intValue();
        }
        target->useAnotherId->setValue(newId);
    }



}

var PresetAction::getValue()
{
    var val = var();
    return val;
}
