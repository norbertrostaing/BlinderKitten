/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "Group.h"
#include "GroupManager.h"
#include "../Command/CommandSelectionManager.h"
#include "../../Brain.h"
#include "UI/GridView/GroupGridView.h"
#include "UserInputManager.h"

Group::Group(var params) :
	BaseItem(params.getProperty("name", "Group")),
	objectType(params.getProperty("type", "Group").toString()),
	objectData(params)
{
	saveAndLoadRecursiveData = true;
	nameCanBeChangedByUser = false;
	canBeDisabled = false;

	itemDataType = "Group";

	id = addIntParameter("ID", "ID of this group", 1, 1);
	userName = addStringParameter("Name", "Name of this group","New group");
	updateName();
	useAnotherId = addIntParameter("Use another", "id of another group to use instead of this one", 0, 0);

	addChildControllableContainer(&selection);
	Brain::getInstance()->registerGroup(this, id->getValue());
	if (params.isVoid()) {
		selection.addItem();
	}
	

}

Group::~Group()
{
	Brain::getInstance()->unregisterGroup(this);
}

void Group::updateName() {
	String n = userName->stringValue();
	if (parentContainer != nullptr) {
		dynamic_cast<GroupManager*>(parentContainer.get())->reorderItems();
	}
	n = String(id->stringValue()) + " - " + n;
	setNiceName(n);
}

void Group::onContainerParameterChangedInternal(Parameter* p) {
	if (p == id) {
		Brain::getInstance()->registerGroup(this, id->getValue(), true);
	}
	if (p == userName || p == id) {
		updateName();
	}
}


void Group::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) {
	Programmer* currentProgrammer = UserInputManager::getInstance()->getProgrammer(false);
	bool programmerNeedRefresh = false;
	int myId = id->getValue();
	if (currentProgrammer != nullptr) {
		for (int iCommand = 0; iCommand < currentProgrammer->commands.items.size() && !programmerNeedRefresh; iCommand++) {
			Command* currentCommand = currentProgrammer->commands.items[iCommand];
			for (int iValue = 0; iValue < currentCommand->selection.items.size() && !programmerNeedRefresh; iValue++) {
				CommandSelection* cs = currentCommand->selection.items[iValue];
				if (cs->targetType->getValue() == "group") {
					int tFromId = cs->valueFrom->getValue();
					int tToId = cs->valueTo->getValue();
					if (tFromId == myId) { programmerNeedRefresh = true; }
					if (tToId == myId) { programmerNeedRefresh = true; }
				}
			}

		}
		if (programmerNeedRefresh) {
			Brain::getInstance()->pleaseUpdate(currentProgrammer);
		}
	}
}
