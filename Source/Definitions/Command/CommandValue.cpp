/*
  ==============================================================================

    CommandTypeChannel.cpp
    Created: 8 Nov 2021 7:28:28pm
    Author:  No

  ==============================================================================
*/

#include "CommandValue.h"
#include "../ChannelFamily/ChannelFamilyManager.h"

CommandValue::CommandValue(var params) :
    BaseItem(params.getProperty("name", "Value")),
    objectType(params.getProperty("type", "CommandValue").toString()),
    objectData(params)

{
    presetOrValue = addEnumParameter("Value type", "What kind of value do you want to apply ?");
    presetOrValue-> addOption("Raw Values", "value");
    presetOrValue->addOption("Preset", "preset");
    presetOrValue->addOption("Release", "release");

    channelType = addTargetParameter("Channel type", "Type of Channel", ChannelFamilyManager::getInstance());
    channelType->maxDefaultSearchLevel = 2;
    channelType->targetType = TargetParameter::CONTAINER;
    channelType->typesFilter.add("ChannelType");

    // release = addBoolParameter("Release value", "release tracked values", false);
    valueFrom = addFloatParameter("Value", "Value of the first element", 0, 0, 1);
    presetIdFrom = addIntParameter("ID From", "ID of the preset for the first element", 0, 0);

    thru = addBoolParameter("Thru", "Do you want to apply to multiple elements ?", false);

    valueTo = addFloatParameter("Value To", "Value of the last element", 0, 0, 1);
    presetIdTo = addIntParameter("ID To", "ID of the preset for the last element", 0, 0);

    symmetry = addBoolParameter("Symmetry", "Apply value with symmetry ?", false);

    HTPOverride = addBoolParameter("HTP Override", "If checked, HTP channels will be computed as LTP.", false);

    stepSize = addFloatParameter("Step size", "Size of the step",1,0);
    stepSize-> hideInEditor = true;
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
    bool rel = presetOrValue->getValue() == "release";

    channelType->hideInEditor = !(val || rel);
    valueFrom->hideInEditor = !val;
    valueTo->hideInEditor = !(th && val);

    presetIdFrom->hideInEditor = !(prst);
    presetIdTo->hideInEditor = !(th && prst);

    symmetry->hideInEditor = !th;

    stepSize->hideInEditor = !shouldShowStepSize;

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

