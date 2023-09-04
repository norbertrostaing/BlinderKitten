/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "Stamp.h"
#include "StampManager.h"
#include "../Command/CommandSelectionManager.h"
#include "../../Brain.h"
#include "UserInputManager.h"

Stamp::Stamp(var params) :
	BaseItem(params.getProperty("name", "Stamp")),
	objectType(params.getProperty("type", "Stamp").toString()),
	objectData(params)
{
	saveAndLoadRecursiveData = true;
	nameCanBeChangedByUser = false;
	canBeDisabled = false;

	itemDataType = "Stamp";

	id = addIntParameter("ID", "ID of this Stamp", 1, 1);
	userName = addStringParameter("Name", "Name of this Stamp","New Stamp");
	updateName();

	addChildControllableContainer(&selection);
	Brain::getInstance()->registerStamp(this, id->getValue());
	if (params.isVoid()) {
		selection.addItem();
	}
}

Stamp::~Stamp()
{
	Brain::getInstance()->unregisterStamp(this);
}

void Stamp::updateName() {
	String n = userName->getValue();
	if (parentContainer != nullptr) {
		dynamic_cast<StampManager*>(parentContainer.get())->reorderItems();
	}
	setNiceName(String((int)id->getValue()) + " - "+n);
}

void Stamp::onContainerParameterChangedInternal(Parameter* p) {
	if (p == userName || p == id) {
		updateName();
	}
	if (p == id) {
		Brain::getInstance()->registerStamp(this, id->getValue(), true);
	}
}


void Stamp::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) {
}
