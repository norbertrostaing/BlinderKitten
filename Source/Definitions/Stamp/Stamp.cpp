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
	objectData(params),
	stampValues("Values")
{
	saveAndLoadRecursiveData = true;
	nameCanBeChangedByUser = false;
	canBeDisabled = false;

	itemDataType = "Stamp";

	id = addIntParameter("ID", "ID of this Stamp", 1, 1);
	userName = addStringParameter("Name", "Name of this Stamp","New Stamp");
	updateName();

	layoutId = addIntParameter("Layout", "Id of desired layout", 0);
	groupId = addIntParameter("Group", "Id of desired group", 0);
	position = addPoint2DParameter("Position", "Layout position, center of image, layout units");
	size = addPoint2DParameter("Size", "Size of the stamp (with layout units)");
	angle = addFloatParameter("Angle", "Angle of the stamp", 0, -360, 360);

	addChildControllableContainer(&stampValues);
	redFull.setNiceName("Red channel at full");
	stampValues.addChildControllableContainer(&redFull);
	redZero.setNiceName("Red channel at zero");
	stampValues.addChildControllableContainer(&redZero);
	greenFull.setNiceName("Green channel at full");
	stampValues.addChildControllableContainer(&greenFull);
	greenZero.setNiceName("Green channel at zero");
	stampValues.addChildControllableContainer(&greenZero);
	blueFull.setNiceName("Blue channel at full");
	stampValues.addChildControllableContainer(&blueFull);
	blueZero.setNiceName("Blue channel at zero");
	stampValues.addChildControllableContainer(&blueZero);
	//addChildControllableContainer(&selection);
	Brain::getInstance()->registerStamp(this, id->getValue());

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
