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
#include "../../Brain.h"

VirtualFaderCol::VirtualFaderCol(var params) :
	BaseItem(params.getProperty("name", "VirtualFaderCol")),
	objectType(params.getProperty("type", "VirtualFaderCol").toString()),
	objectData(params),
	rotaries("Rotaries"),
	aboveButtons("Above Buttons"),
	fader("Fader"),
	belowButtons("Below Buttons")
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

	addChildControllableContainer(&rotaries);
	rotaries.selectItemWhenCreated = false;
	addChildControllableContainer(&aboveButtons);
	aboveButtons.selectItemWhenCreated = false;
	addChildControllableContainer(&fader);
	belowButtons.selectItemWhenCreated = false;
	addChildControllableContainer(&belowButtons);

	updateDisplay();
	updateName();
	
}

VirtualFaderCol::~VirtualFaderCol()
{
}

void VirtualFaderCol::updateName() {
}

void VirtualFaderCol::onContainerParameterChangedInternal(Parameter* c) {
	VirtualFaderColGrid::getInstance()->fillCells();
	if (c == targetType || c == targetId) {
		updateDisplay();
	}
}

void VirtualFaderCol::updateDisplay() {
	String targType = targetType->getValue();

	targetId->deselectThis();
	for (int i = 0; i < rotaries.items.size(); i++) { rotaries.items[i]->updateDisplay(); }
	for (int i = 0; i < aboveButtons.items.size(); i++) { aboveButtons.items[i]->updateDisplay(); }
	fader.updateDisplay();
	for (int i = 0; i < belowButtons.items.size(); i++) { belowButtons.items[i]->updateDisplay(); }
	
    //queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));

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

String VirtualFaderCol::getTargetName() {
	String text = "";
	String targType = targetType->getValue();
	int targId = targetId->getValue();
	if (targId == 0) { return ""; }
	if (targType == "cuelist") {
		Cuelist* targ = Brain::getInstance()->getCuelistById(targId);
		if (targ != nullptr) { text = targ->userName->getValue(); }
	}
	else if (targType == "effect") {
		Effect* targ = Brain::getInstance()->getEffectById(targId);
		if (targ != nullptr) { text = targ->userName->getValue(); }
	}
	else if (targType == "carousel") {
		Carousel* targ = Brain::getInstance()->getCarouselById(targId);
		if (targ != nullptr) { text = targ->userName->getValue(); }
	}

	return text;
}

String VirtualFaderCol::getTargetType() {
	String targType = targetType->getValue();
	int targId = targetId->getValue();
	if (targId == 0) { return ""; }
	if (targType == "cuelist") {
		Cuelist* targ = Brain::getInstance()->getCuelistById(targId);
		if (targ != nullptr) { return "cuelist"; }
	}
	else if (targType == "effect") {
		Effect* targ = Brain::getInstance()->getEffectById(targId);
		if (targ != nullptr) { return "effect"; }
	}
	else if (targType == "carousel") {
		Carousel* targ = Brain::getInstance()->getCarouselById(targId);
		if (targ != nullptr) { return "carousel"; }
	}

	return "";
}

void VirtualFaderCol::loadJSONDataInternal(var data) {
	BaseItem::loadJSONDataInternal(data);
	updateDisplay();
}
