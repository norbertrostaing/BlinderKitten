/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "VirtualFaderCol.h"
#include "VirtualFaderColManager.h"
#include "VirtualFaderColGrid.h"
#include "VirtualFaderButton.h"
#include "VirtualFaderSlider.h"
#include "../../Brain.h"

VirtualFaderCol::VirtualFaderCol(var params) :
	BaseItem(params.getProperty("name", "VirtualFaderCol")),
	objectType(params.getProperty("type", "VirtualFaderCol").toString()),
	objectData(params)
{
	saveAndLoadRecursiveData = true;
	nameCanBeChangedByUser = false;

	itemDataType = "VirtualFaderCol";

	pageNumber = addIntParameter("Page number", "", 0,0);
	colNumber = addIntParameter("Col number", "", 0,0);

	targetType = addEnumParameter("Target type", "");
	targetType->addOption("Cuelist", "cuelist");
	targetType->addOption("Effect", "effect");
	targetType->addOption("Carousel", "carousel");

	targetId = addIntParameter("Target ID", "", 0, 0);
	updateDisplay();
	updateName();

}

VirtualFaderCol::~VirtualFaderCol()
{
}

void VirtualFaderCol::updateName() {
}

void VirtualFaderCol::onContainerParameterChangedInternal(Parameter* c) {
	if (c == targetType) {
		updateDisplay();
	}
	VirtualFaderColGrid::getInstance()->fillCells();
}

void VirtualFaderCol::updateDisplay() {
	String targType = targetType->getValue();

    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

void VirtualFaderCol::pressed() { 
	String targType = targetType->getValue();
	int targId = targetId->getValue();
	if (targId == 0) {return;}

}

void VirtualFaderCol::released() { 
	String targType = targetType->getValue();
	int targId = targetId->getValue();
	if (targId == 0) { return; }

}

String VirtualFaderCol::getBtnText() {
	String text = "";
	String targType = targetType->getValue();
	String action = "";
	int targId = targetId->getValue();
	if (targId == 0) { return ""; }
	
	if (text != "") {
		text = action + "\n" + text;
	}
	return text;
}
