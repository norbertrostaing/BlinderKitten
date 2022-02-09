/*
  ==============================================================================

    CommandTypeChannel.cpp
    Created: 8 Nov 2021 7:28:28pm
    Author:  No

  ==============================================================================
*/

#include "CommandSelection.h"

CommandSelection::CommandSelection(var params) :
    BaseItem(params.getProperty("name", "Selection")),
    objectType(params.getProperty("type", "CommandSelection").toString()),
    objectData(params)
{
    plusOrMinus = addEnumParameter("Action", "Do you wan to add or remove elements to selection ?");
    plusOrMinus->addOption("+", "add");
    plusOrMinus->addOption("-", "remove");

    targetType = addEnumParameter("Target type", "What kinf of element are you targetting ?");
    targetType->addOption("Fixture", "fixture");
    targetType->addOption("Group", "group");
    targetType->addOption("Device", "device");
    valueFrom = addIntParameter("ID", "ID of the first element",1,1);
    thru = addBoolParameter("Thru", "Do you want to add multiple elements ?", false);
    valueTo = addIntParameter("Last ID", "ID of the last element (only if thru checked)", 1, 1);

    filter = addEnumParameter("Filter", "What kind of filter do yuo want to apply to selection ?");
    filter->addOption("None", "none");
    filter->addOption("Divide", "divide");
    filter->addOption("Pattern", "pattern");
    pattern = addStringParameter("Pattern", "type 1 to select fixtures and 0 to skip them", "");
    symmetry = addBoolParameter("Symmetry", "Apply this pattern with symmetry", false);
    
    updateDisplay();
};

CommandSelection::~CommandSelection()
{
};

void CommandSelection::updateDisplay()
{
    bool th = thru->getValue();
    bool pat = filter->getValue() != "none";

    valueTo -> hideInEditor = !th;
    filter -> hideInEditor = !th;
    pattern -> hideInEditor = !(th && pat);
    symmetry -> hideInEditor = !(th && pat);

    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

void CommandSelection::onContainerParameterChangedInternal(Parameter* p) {
    if (p == thru || p == filter) {
        updateDisplay();
    }
}

void CommandSelection::afterLoadJSONDataInternal() {
    updateDisplay();
}

