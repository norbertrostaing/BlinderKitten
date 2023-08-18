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
    valueFrom = addIntParameter("ID", "ID of the first element",0,0);
    thru = addBoolParameter("Thru", "Do you want to add multiple elements ?", false);
    valueTo = addIntParameter("Last ID", "ID of the last element (only if thru checked)", 0, 0);

    subSel = addBoolParameter("SubFixtures", "select only some subfixtures ?",false);
    subFrom = addIntParameter("SubFixtures From", "First id of subFixture",0,0);
    subThru = addBoolParameter("SubFixtures Thru", "select multiple subfixture ?",false);
    subTo = addIntParameter("SubFixtures To", "Last id of subFixture", 0, 0);

    filter = addEnumParameter("Filter", "What kind of filter do yuo want to apply to selection ?");
    filter->addOption("None", "none");
    filter->addOption("Reverse", "reverse");
    filter->addOption("Divide", "divide");
    filter->addOption("Pattern", "pattern");
    filter->addOption("Shuffle", "shuffle");
    filter->addOption("Pick Random", "random");
    filter->addOption("Layout Direction", "layoutdir");
    pattern = addStringParameter("Pattern", "type 1 to select SubFixtures and 0 to skip them, for example, 100 will select every first SubFixture of three in pattern mode, and the first third of all SubFixtures in divide mode", "");
    randomSeed = addIntParameter("Seed", "Seed used to generate random, if 0, selection will change each call, if not, the random selection will alway be the same", 0, 0);
    randomNumber = addIntParameter("Number of subfixtures", "Number of subfixtures to take randomly", 1, 1);
    randomBuddy = addIntParameter("Buddying", "They stay together", 1, 1);
    randomBlock = addIntParameter("Block", "Repetitions", 1, 1);
    randomWing = addIntParameter("Wings", "Symmetries", 1, 1);
    layoutId = addIntParameter("Layout ID", "Id ot desired layout", 0, 0);
    layoutDirection = addFloatParameter("Direction angle", "angle of selection direction", 0, -360, 360);
    symmetry = addBoolParameter("Symmetry", "Apply this pattern with symmetry", false);

    updateDisplay();
};

CommandSelection::~CommandSelection()
{
};

void CommandSelection::updateDisplay()
{
    bool th = thru->getValue();
    bool mult = thru->getValue() || targetType->getValue()=="group";
    bool pat = filter->getValue() == "divide" || filter->getValue() == "pattern";
    bool randSeed = filter->getValue() == "shuffle" || filter->getValue() == "random";
    bool randNum = filter->getValue() == "random";
    bool layout = filter->getValue() == "layoutdir";
    bool layoutDir = filter->getValue() == "layoutdir";

    randSeed = randSeed && mult;
    randNum = randNum && mult;
    
    bool isFixture = targetType->getValue() == "fixture";


    valueTo -> hideInEditor = !th;
    filter -> hideInEditor = false;
    pattern -> hideInEditor = !(pat);
    symmetry -> hideInEditor = !(pat) && !layout;

    bool sub = subSel->getValue();
    bool subTh = subThru->getValue();

    subFrom -> hideInEditor = !sub;
    subThru->hideInEditor = !sub;
    subTo->hideInEditor = !(sub && subTh);

    randomSeed -> hideInEditor = !randSeed;
    randomNumber->hideInEditor = !randNum;
    randomBuddy->hideInEditor = !randNum;
    randomBlock->hideInEditor = !randNum;
    randomWing->hideInEditor = !randNum;

    layoutId->hideInEditor = !layout;
    layoutDirection->hideInEditor = !layoutDir;

    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

void CommandSelection::onContainerParameterChangedInternal(Parameter* p) {
    if (p == thru || p == targetType || p == filter || p == subSel || p == subThru) {
        updateDisplay();
    }
}

void CommandSelection::afterLoadJSONDataInternal() {
    updateDisplay();
}

