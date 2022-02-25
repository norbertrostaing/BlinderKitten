/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "VirtualButton.h"
#include "VirtualButtonManager.h"
#include "../../Brain.h"

VirtualButton::VirtualButton(var params) :
	BaseItem(params.getProperty("name", "VirtualButton")),
	objectType(params.getProperty("type", "VirtualButton").toString()),
	objectData(params)
{
	saveAndLoadRecursiveData = true;
	nameCanBeChangedByUser = false;

	itemDataType = "VirtualButton";

	pageNumber = addIntParameter("Page number", "", 0,0);
	rowNumber = addIntParameter("Row number", "", 0,0);
	colNumber = addIntParameter("Col number", "", 0,0);

	targetType = addEnumParameter("Target type", "");
	targetType->addOption("Cuelist", "cuelist");
	targetType->addOption("Effect", "effect");
	targetType->addOption("Carousel", "carousel");

	targetId = addIntParameter("Target ID", "", 0, 0);
	cuelistAction = addEnumParameter("Cuelist action", "");
	cuelistAction->addOption("Go", "go");
	cuelistAction->addOption("Off", "off");
	cuelistAction->addOption("Flash", "flash");
	cuelistAction->addOption("Swop", "swop");
	cuelistAction->addOption("Go random", "gorandom");

	effectAction = addEnumParameter("Effect action", "");
	effectAction->addOption("Start", "start");
	effectAction->addOption("Stop", "stop");
	effectAction->addOption("Tap tempo", "taptempo");

	carouselAction = addEnumParameter("Carousel Action", "");
	carouselAction->addOption("Start", "start");
	carouselAction->addOption("Stop", "stop");
	carouselAction->addOption("Tap tempo", "taptempo");

	// id = addIntParameter("ID", "ID of this VirtualButton", 1, 1);
	// userName = addStringParameter("Name", "Name of this VirtualButton","New VirtualButton");
	updateDisplay();
	updateName();

}

VirtualButton::~VirtualButton()
{
}

void VirtualButton::updateName() {
}

void VirtualButton::onContainerParameterChangedInternal(Parameter* c) {
	if (c == targetType) {
		updateDisplay();
	}
}

void VirtualButton::updateDisplay() {
	String targType = targetType->getValue();

	cuelistAction->hideInEditor = targType != "cuelist";
	effectAction->hideInEditor = targType != "effect";
	carouselAction->hideInEditor = targType != "carousel";

    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

void VirtualButton::pressed() { 

}
void VirtualButton::released() { 

}


