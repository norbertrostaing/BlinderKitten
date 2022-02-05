/*
  ==============================================================================

    CommandTypeChannel.cpp
    Created: 8 Nov 2021 7:28:28pm
    Author:  No

  ==============================================================================
*/

#include "CommandValue.h"
#include "../FixtureParamType/FixtureParamTypeManager.h"

CommandValue::CommandValue() :
    BaseItem("Value")
{
    presetOrValue = addEnumParameter("Value type", "What kind of value do you want to apply ?");
    presetOrValue-> addOption("Raw Values", "value");
    presetOrValue->addOption("Preset", "preset");
    presetOrValue->addOption("Release", "release");

    channelType = addTargetParameter("Channel type", "Type of Channel", FixtureParamTypeManager::getInstance());
    channelType->maxDefaultSearchLevel = 2;
    channelType->targetType = TargetParameter::CONTAINER;

    presetType = addTargetParameter("Preset family", "Type of preset", FixtureParamTypeManager::getInstance());
    presetType -> maxDefaultSearchLevel = 0;
    presetType -> targetType = TargetParameter::CONTAINER;

    // release = addBoolParameter("Release value", "release tracked values", false);
    valueFrom = addFloatParameter("Value", "Value of the first element", 0, 0, 1);
    presetIdFrom = addIntParameter("ID", "ID of the preset for the first element", 1, 1);

    thru = addBoolParameter("Thru", "Do you want to apply to multiple elements ?", false);

    valueTo = addFloatParameter("Value To", "Value of the last element", 0, 0, 1);
    presetIdTo = addIntParameter("ID", "ID of the preset for the last element", 1, 1);

    symmetry = addBoolParameter("Symmetry", "Apply value with symmetry ?", false);
    updateDisplay();
};

CommandValue::~CommandValue()
{
};

void CommandValue::updateDisplay() 
{
    bool val = presetOrValue->getValue() == "value";
    bool prst = presetOrValue->getValue() == "preset";
    bool th = thru->getValue();

    channelType->hideInEditor = !(val);
    valueFrom->hideInEditor = !val;
    valueTo->hideInEditor = !(th && val);

    presetType->hideInEditor = !(prst);
    presetIdFrom->hideInEditor = !(prst);
    presetIdTo->hideInEditor = !(th && prst);

    symmetry->hideInEditor = !th;

    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

void CommandValue::onContainerParameterChangedInternal(Parameter* p) {
    if (p == thru || p == presetOrValue) {
        updateDisplay();
    }
}

void CommandValue::afterLoadJSONDataInternal() {
    BaseItem::afterLoadJSONDataInternal();
    updateDisplay();
}

