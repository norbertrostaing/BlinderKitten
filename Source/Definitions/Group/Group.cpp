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

Group::Group(var params) :
	BaseItem(params.getProperty("name", "Group")),
	objectType(params.getProperty("type", "Group").toString()),
	objectData(params)
{
	saveAndLoadRecursiveData = true;
	nameCanBeChangedByUser = false;

	itemDataType = "Group";

	id = addIntParameter("ID", "ID of this group", 1, 1);
	userName = addStringParameter("Name", "Name of this group","New group");
	updateName();

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
	String n = userName->getValue();
	if (parentContainer != nullptr) {
		dynamic_cast<GroupManager*>(parentContainer.get())->reorderItems();
	}
	setNiceName(String((int)id->getValue()) + " - "+n);
}

void Group::onContainerParameterChangedInternal(Parameter* p) {
	if (p == userName || p == id) {
		updateName();
	}
	if (p == id) {
		Brain::getInstance()->registerGroup(this, id->getValue());
	}
	GroupGridView::getInstance()->updateCells();
}
