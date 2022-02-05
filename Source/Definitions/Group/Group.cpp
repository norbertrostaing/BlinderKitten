/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "Group.h"
#include "../Command/CommandSelectionManager.h"
#include "../../Brain.h"

Group::Group(var params) :
	BaseItem(params.getProperty("name", "Group")),
	objectType(params.getProperty("type", "Group").toString()),
	objectData(params)
{
	saveAndLoadRecursiveData = true;

	itemDataType = "Group";

	id = addIntParameter("ID", "ID of this device", 1, 1);
	selection = new CommandSelectionManager();
	addChildControllableContainer(selection);

	Brain::getInstance()->registerGroup(this, id->getValue());
}

Group::~Group()
{
	Brain::getInstance()->unregisterGroup(this);
}

void Group::parameterValueChanged(Parameter* p) {
	BaseItem::parameterValueChanged(p);
	Logger::writeToLog("changed !");
	if (p == id) {
		Brain::getInstance()->registerGroup(this, id->getValue());
	}
}
