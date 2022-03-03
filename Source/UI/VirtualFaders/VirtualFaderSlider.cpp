/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "VirtualFaderSlider.h"
#include "VirtualFaderColManager.h"
#include "VirtualFaderColGrid.h"
#include "../../Brain.h"

VirtualFaderSlider::VirtualFaderSlider(var params) :
	BaseItem(params.getProperty("name", "VirtualFaderSlider")),
	objectType(params.getProperty("type", "VirtualFaderSlider").toString()),
	objectData(params)
{
	saveAndLoadRecursiveData = true;
	nameCanBeChangedByUser = false;

	itemDataType = "VirtualFaderSlider";

	targetType = addEnumParameter("Target type", "");
	targetType->addOption("disabled", "disabled");
	targetType->addOption("Same as column", "column");
	targetType->addOption("Cuelist", "cuelist");
	targetType->addOption("Effect", "effect");
	targetType->addOption("Carousel", "carousel");

	targetId = addIntParameter("Target ID", "", 0, 0);
	cuelistAction = addEnumParameter("Cuelist action", "");
	cuelistAction->addOption("HTP Level", "htplevel");
	cuelistAction->addOption("Flash Level", "flashlevel");

	effectAction = addEnumParameter("Effect action", "");
	effectAction->addOption("Size", "size");
	effectAction->addOption("Speed", "speed");

	carouselAction = addEnumParameter("Carousel Action", "");
	carouselAction->addOption("Size", "size");
	carouselAction->addOption("Speed", "speed");

	updateDisplay();
	updateName();
}

VirtualFaderSlider::~VirtualFaderSlider()
{
}

void VirtualFaderSlider::updateName() {
}

void VirtualFaderSlider::onContainerParameterChangedInternal(Parameter* c) {
	if (c == targetType) {
		updateDisplay();
	}
	VirtualFaderColGrid::getInstance()->fillCells();
}

void VirtualFaderSlider::updateDisplay() {
	String targType = targetType->getValue();

	targetId->hideInEditor = targType == "column" || targType == "disabled";

	if (targType == "column" && parentContainer != nullptr && parentContainer -> parentContainer != nullptr) {
		if (parentContainer->niceName == "Rotaries") {
			targType = dynamic_cast<VirtualFaderCol*>(parentContainer->parentContainer.get())->targetType->getValue();
		}
		else { // fader
			targType = dynamic_cast<VirtualFaderCol*>(parentContainer.get())->targetType->getValue();
		}
	}

	cuelistAction->hideInEditor = targType != "cuelist";
	effectAction->hideInEditor = targType != "effect";
	carouselAction->hideInEditor = targType != "carousel";

	queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

void VirtualFaderSlider::moved(float value, String colTargetType, int colTargetId) {
	String targType = targetType->getValue();
	int targId = targetId->getValue();

	if (targType == "column") {
		targType = colTargetType;
		targId = colTargetId;
	}

	if (targId == 0) {return;}

	if (targType == "cuelist") {
		Cuelist* targ = Brain::getInstance()->getCuelistById(targId);
		if (targ != nullptr) {
			String action = cuelistAction->getValue();
			if (action == "htplevel") { targ->HTPLevel->setValue(value); }
			if (action == "flashlevel") { targ->FlashLevel->setValue(value); }
		}
	}
	else if (targType == "effect") {
		Effect* targ = Brain::getInstance()->getEffectById(targId);
		if (targ != nullptr) {
			String action = effectAction->getValue();
			if (action == "size") { targ->sizeValue->setValue(value); }
			if (action == "speed") { targ->speed->setValue(value); }
		}
	}
	else if (targType == "carousel") {
		Carousel* targ = Brain::getInstance()->getCarouselById(targId);
		if (targ != nullptr) {
			String action = carouselAction->getValue();
			if (action == "size") { targ->sizeValue->setValue(value); }
			if (action == "speed") { targ->speed->setValue(value); }
		}
	}

}

void VirtualFaderSlider::released() { 
	String targType = targetType->getValue();
	int targId = targetId->getValue();
	if (targId == 0) { return; }

	if (targType == "cuelist") {
		Cuelist* targ = Brain::getInstance()->getCuelistById(targId);
		if (targ != nullptr) {
			String action = cuelistAction->getValue();
			if (action == "flash") { targ->flash(false, false, false); }
			if (action == "swop") { targ->flash(false, false, true); }
		}
	}
	else if (targType == "effect") {
		Effect* targ = Brain::getInstance()->getEffectById(targId);
		if (targ != nullptr) {
			String action = effectAction->getValue();
			// if (action == "start") { targ->start(); }
		}
	}
	else if (targType == "carousel") {
		Carousel* targ = Brain::getInstance()->getCarouselById(targId);
		if (targ != nullptr) {
			String action = carouselAction->getValue();
			// if (action == "start") { targ->start(); }
		}
	}

}

String VirtualFaderSlider::getBtnText(String columnType) {
	String text = "";
	String targType = targetType->getValue();
	String action = "";
	if (targType == "column" && columnType != "") {
		targType = columnType;
		if (targType == "cuelist") {
			action = cuelistAction->getValue();
		}
		else if (targType == "effect") {
			action = effectAction->getValue();
		}
		else if (targType == "carousel") {
			action = carouselAction->getValue();
		}

		return action;
	}
	else {
		int targId = targetId->getValue();
		if (targId == 0) { return ""; }

		if (targType == "cuelist") {
			Cuelist* targ = Brain::getInstance()->getCuelistById(targId);
			action = cuelistAction->getValue();
			if (targ != nullptr) {
				text = targ->userName->getValue();
			}
		}
		else if (targType == "effect") {
			Effect* targ = Brain::getInstance()->getEffectById(targId);
			action = effectAction->getValue();
			if (targ != nullptr) {
				text = targ->userName->getValue();
			}
		}
		else if (targType == "carousel") {
			Carousel* targ = Brain::getInstance()->getCarouselById(targId);
			action = carouselAction->getValue();
			if (targ != nullptr) {
				text = targ->userName->getValue();
			}
		}

		if (text != "") {
			text = action + "\n" + text;
		}
		return text;
	}
}
