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
#include "Definitions/TimingPreset/TimingPreset.h"

PresetAction::PresetAction(var params) :
    Action(params)
{
    actionType = (ActionType)(int)params.getProperty("actionType", TIMING_PRESET_SET);

    targetId = addIntParameter("Preset id", "Id of the new target", 0, 0);

    if (actionType == TIMING_PRESET_SET) {
        component = addEnumParameter("Component", "");
        component->addOption("Delay from", "dFrom")->addOption("Delay to", "dTo")->addOption("Fade from", "fFrom")->addOption("Fade to", "fTo");

        fromValue = addFloatParameter("From value", "Low value", 0);
        toValue = addFloatParameter("To value", "High value", 0);
    }
    else if (actionType == TIMING_PRESET_FIXED) {
        component = addEnumParameter("Component", "");
        component->addOption("Delay from", "dFrom")->addOption("Delay to", "dTo")->addOption("Fade from", "fFrom")->addOption("Fade to", "fTo");
        fixedValue = addFloatParameter("Value", "Value", 0);
    }
}

PresetAction::~PresetAction()
{
}

void PresetAction::onContainerParameterChangedInternal(Parameter* p)
{
}

void PresetAction::updateDisplay()
{
    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}


void PresetAction::triggerInternal()
{
}

void PresetAction::setValueInternal(var value, String origin, int indexIncrement, bool isRelative) {
    float val = value;

    if (actionType == TIMING_PRESET_SET) {
        TimingPreset* target = Brain::getInstance()->getTimingPresetById(targetId->intValue());
        String comp = component->getValueData().toString();
        if (target == nullptr) { return; }
        float newVal = jmap(val, 0.f, 1.f, fromValue->floatValue(), toValue->floatValue());
        if (comp == "dFrom") target->delayFrom->setValue(newVal);
        else if (comp == "dTo") target->delayTo->setValue(newVal);
        else if (comp == "fFrom") target->fadeFrom->setValue(newVal);
        else if (comp == "fTo") target->fadeTo->setValue(newVal);
    }

    else if (actionType == TIMING_PRESET_FIXED) {
        if (val < 1) {
            return;
        }
        TimingPreset* target = Brain::getInstance()->getTimingPresetById(targetId->intValue());
        String comp = component->getValueData().toString();
        if (target == nullptr) { return; }
        float newVal = fixedValue->floatValue();
        if (comp == "dFrom") target->delayFrom->setValue(newVal);
        else if (comp == "dTo") target->delayTo->setValue(newVal);
        else if (comp == "fFrom") target->fadeFrom->setValue(newVal);
        else if (comp == "fTo") target->fadeTo->setValue(newVal);
    }



}

var PresetAction::getValue()
{
    var val = var();
    if (actionType == TIMING_PRESET_SET) {
        TimingPreset* target = Brain::getInstance()->getTimingPresetById(targetId->intValue());
        String comp = component->getValueData().toString();
        if (target == nullptr) { return val; }
        if (comp == "dFrom") val = target->delayFrom->floatValue();
        else if (comp == "dTo") val = target->delayTo->floatValue();
        else if (comp == "fFrom") val = target->fadeFrom->floatValue();
        else if (comp == "fTo") val = target->fadeTo->floatValue();
        val = jmap((float)val, fromValue->floatValue(), toValue->floatValue(), 0.f, 1.f);
    }

    return val;
}
