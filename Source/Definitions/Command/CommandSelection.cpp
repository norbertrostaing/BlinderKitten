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
    targetType->addOption("Fixture", "Fixture");
    targetType->addOption("Group", "group");
    valueFrom = addIntParameter("ID", "ID of the first element",1,1);
    thru = addBoolParameter("Thru", "Do you want to add multiple elements ?", false);
    valueTo = addIntParameter("Last ID", "ID of the last element (only if thru checked)", 1, 1);

    subSel = addBoolParameter("SubFixtures", "select only some subfixtures ?",false);
    subFrom = addIntParameter("SubFixtures From", "First id of subFixture",0,0);
    subThru = addBoolParameter("SubFixtures Thru", "select multiple subfixture ?",false);
    subTo = addIntParameter("SubFixtures To", "Last id of subFixture", 0, 0);

    filter = addEnumParameter("Filter", "What kind of filter do yuo want to apply to selection ?");
    filter->addOption("None", "none");
    filter->addOption("Divide", "divide");
    filter->addOption("Pattern", "pattern");
    pattern = addStringParameter("Pattern", "type 1 to select SubFixtures and 0 to skip them, for example, 100 will select every first SubFixture of three in pattern mode, and the first third of all SubFixtures in divide mode", "");
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
    bool sub = subSel->getValue();
    bool subTh = subThru->getValue();


    valueTo -> hideInEditor = !th;
    filter -> hideInEditor = !th;
    pattern -> hideInEditor = !(th && pat);
    symmetry -> hideInEditor = !(th && pat);

    subFrom -> hideInEditor = !sub;
    subThru->hideInEditor = !sub;
    subTo->hideInEditor = !(sub && subTh);

    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

void CommandSelection::onContainerParameterChangedInternal(Parameter* p) {
    if (p == thru || p == filter || p == subSel || p == subThru) {
        updateDisplay();
    }
}

void CommandSelection::afterLoadJSONDataInternal() {
    updateDisplay();
}

