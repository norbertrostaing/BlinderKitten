/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "VirtualFaderSlider.h"
#include "VirtualFaderCol.h"
#include "VirtualFaderColManager.h"
#include "VirtualFaderColGrid.h"
#include "../../Brain.h"

VirtualFaderSlider::VirtualFaderSlider(var params) :
	BaseItem(params.getProperty("name", "VirtualFaderSlider")),
	objectType(params.getProperty("type", "VirtualFaderSlider").toString()),
	objectData(params),
	actionManager("Generic Actions")
{
	saveAndLoadRecursiveData = true;
	nameCanBeChangedByUser = false;
	canBeDisabled = false;

	itemDataType = "VirtualFaderSlider";

	targetType = addEnumParameter("Target type", "");
	targetType->addOption("disabled", "disabled");
	targetType->addOption("Same as column", "column");
	targetType->addOption("Cuelist", "cuelist");
	targetType->addOption("Effect", "effect");
	targetType->addOption("Carousel", "carousel");
	targetType->addOption("Mapper", "mapper");
	targetType->addOption("Generic Actions", "actions");

	targetId = addIntParameter("Target ID", "", 0, 0);
	cuelistAction = addEnumParameter("Cuelist action", "");
	cuelistAction->addOption("HTP Level", "htplevel");
	cuelistAction->addOption("Flash Level", "flashlevel");
	cuelistAction->addOption("LTP Level", "ltplevel");
	cuelistAction->addOption("Chaser Speed", "speed");

	effectAction = addEnumParameter("Effect action", "");
	effectAction->addOption("Size", "size");
	effectAction->addOption("Speed", "speed");

	carouselAction = addEnumParameter("Carousel Action", "");
	carouselAction->addOption("Size", "size");
	carouselAction->addOption("Speed", "speed");

	maxSpeed = addFloatParameter("Max Speed","Speed when your fader is up high",600,0);

	mapperAction = addEnumParameter("Carousel Action", "");
	mapperAction->addOption("Size", "size");

	addChildControllableContainer(&actionManager);

	updateDisplay();
	updateName();
}

VirtualFaderSlider::~VirtualFaderSlider()
{
	VirtualFaderColGrid::getInstance()->sliderToVFS.removeValue(this);

}

void VirtualFaderSlider::updateName() {
}

void VirtualFaderSlider::onContainerParameterChangedInternal(Parameter* c) {
	if (c == targetType || c == cuelistAction || c == carouselAction || c == effectAction) {
		updateDisplay();
	}
	VirtualFaderColGrid::getInstance()->fillCells();
}

void VirtualFaderSlider::updateDisplay() {
	String targType = targetType->getValue();

	targetId->hideInEditor = targType == "column" || targType == "disabled" || targType == "actions";

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
	mapperAction->hideInEditor = targType != "mapper";

	actionManager.hideInEditor = targType != "actions";
	targetId->hideInEditor = targType == "actions";

	bool needSpeed = false;
	needSpeed = needSpeed || (targType == "cuelist" && cuelistAction->getValue() == "speed");
	needSpeed = needSpeed || (targType == "effect" && effectAction->getValue() == "speed");
	needSpeed = needSpeed || (targType == "carousel" && carouselAction->getValue() == "speed");

	maxSpeed -> hideInEditor = !needSpeed;

	queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

float VirtualFaderSlider::getTargetValue(String colTargetType, int colTargetId)
{
	String targType = targetType->getValue();
	int targId = targetId->getValue();

	if (targType == "column") {
		targType = colTargetType;
		targId = colTargetId;
	}

	if (targId == 0) { return 0; }

	if (targType == "cuelist") {
		Cuelist* targ = Brain::getInstance()->getCuelistById(targId);
		if (targ != nullptr) {
			String action = cuelistAction->getValue();
			if (action == "htplevel") { 
				FloatParameter* t = targ->HTPLevel;
				return t->getValue(); 
			}
			if (action == "flashlevel") { return targ->FlashLevel->getValue(); }
			if (action == "ltplevel") { return targ->LTPLevel->getValue(); }
			if (action == "speed") {
				float maxSpeedVal = maxSpeed->floatValue();
				if (maxSpeedVal > 0) {
					return targ->chaserSpeed->floatValue() / maxSpeedVal;
				}
				else {
					return 0;
				}
			}
		}
	}
	else if (targType == "effect") {
		Effect* targ = Brain::getInstance()->getEffectById(targId);
		if (targ != nullptr) {
			String action = effectAction->getValue();
			if (action == "size") { return targ->sizeValue->getValue(); }
			if (action == "speed") {
				float maxSpeedVal = maxSpeed->floatValue();
				if (maxSpeedVal > 0) {
					return targ->speed->floatValue() / maxSpeedVal;
				}
				else {
					return 0;
				}
			}
		}
	}
	else if (targType == "carousel") {
		Carousel* targ = Brain::getInstance()->getCarouselById(targId);
		if (targ != nullptr) {
			String action = carouselAction->getValue();
			if (action == "size") { return targ->sizeValue->getValue(); }
			if (action == "speed") {
				float maxSpeedVal = maxSpeed->floatValue();
				if (maxSpeedVal > 0) {
					return targ->speed->floatValue() / maxSpeedVal;
				}
				else {
					return 0;
				}
			}
		}
	}
	else if (targType == "mapper") {
		Mapper* targ = Brain::getInstance()->getMapperById(targId);
		if (targ != nullptr) {
			String action = mapperAction->getValue();
			if (action == "size") { return targ->sizeValue->getValue(); }
		}
	}
	return 0;

}

void VirtualFaderSlider::moved(float value, String colTargetType, int colTargetId, String origin) {
	String targType = targetType->getValue();
	if (!isAllowedToMove(origin, value)) {
		LOG("not allowed " << origin);
		return;
	}
	if (targType == "actions") {
		actionManager.setValueAll(value, "VirtualFaders");
		return;
	}
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
			if (action == "htplevel") { 
				if (origin == "" || targ->currentHTPLevelController == origin || abs(targ->HTPLevel->floatValue() - value) < 0.05) {
					targ->nextHTPLevelController = origin;
					targ->HTPLevel->setValue(value);
				}
			}
			if (action == "flashlevel") { 
				if (origin == "" || targ->currentFlashLevelController == origin || abs(targ->FlashLevel->floatValue() - value) < 0.05) {
					targ->nextLTPLevelController = origin;
					targ->FlashLevel->setValue(value);
				}
			}
			if (action == "ltplevel") { 
				if (origin == "" || targ->currentLTPLevelController == origin || abs(targ->LTPLevel->floatValue() - value) < 0.05) {
					targ->nextFlashLevelController = origin;
					targ->LTPLevel->setValue(value);
				}
			}
			if (action == "speed") {
				targ->chaserSpeed->setValue(value*maxSpeed->floatValue());
			}
		}
	}
	else if (targType == "effect") {
		Effect* targ = Brain::getInstance()->getEffectById(targId);
		if (targ != nullptr) {
			String action = effectAction->getValue();
			if (action == "size") { 
				if (origin == "" || targ->currentSizeController == origin || abs(targ->sizeValue->floatValue() - value) < 0.05) {
					targ->nextSizeController = origin;
					targ->sizeValue->setValue(value);
				}
			}
			if (action == "speed") {
				targ->speed->setValue(value * maxSpeed->floatValue());
			}
		}
	}
	else if (targType == "carousel") {
		Carousel* targ = Brain::getInstance()->getCarouselById(targId);
		if (targ != nullptr) {
			String action = carouselAction->getValue();
			if (action == "size") { 
				if (origin == "" || targ->currentSizeController == origin || abs(targ->sizeValue->floatValue() - value) < 0.05) {
					targ->nextSizeController = origin;
					targ->sizeValue->setValue(value);
				}
			}
			if (action == "speed") {
				targ->speed->setValue(value * maxSpeed->floatValue());
			}
		}
	}
	else if (targType == "mapper") {
		Mapper* targ = Brain::getInstance()->getMapperById(targId);
		if (targ != nullptr) {
			String action = mapperAction->getValue();
			if (action == "size") { 
				if (origin == "" || targ->currentSizeController == origin || abs(targ->sizeValue->floatValue() - value) < 0.05) {
					targ->nextSizeController = origin;
					targ->sizeValue->setValue(value);
				}
			}
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
	else if (targType == "mapper") {
		Mapper* targ = Brain::getInstance()->getMapperById(targId);
		if (targ != nullptr) {
			String action = mapperAction->getValue();
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
		else if (targType == "mapper") {
			action = mapperAction->getValue();
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
		else if (targType == "mapper") {
			Mapper* targ = Brain::getInstance()->getMapperById(targId);
			action = mapperAction->getValue();
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

bool VirtualFaderSlider::checkParentColumn()
{
	if (parentColumn == nullptr) {
		parentColumn = dynamic_cast<VirtualFaderCol*>(parentContainer->parentContainer.get());
	}
	return parentColumn != nullptr;
}

bool VirtualFaderSlider::isAllowedToMove(String origin, float newValue)
{
	String targType = targetType->getValue();
	if (targType == "actions") {
		return true;
	}

	if (!checkParentColumn()) {
		return false;
	}

	int targId = targetId->getValue();

	if (targType == "column") {
		targType = parentColumn->targetType->getValue();
		targId = parentColumn->targetId->intValue();
	}

	if (targType == "disabled") { return true; }
	if (targId == 0) { return true; }

	if (targType == "cuelist") {
		Cuelist* targ = Brain::getInstance()->getCuelistById(targId);
		if (targ != nullptr) {
			String action = cuelistAction->getValue();
			if (action == "htplevel") {
				// LOG(targ->currentHTPLevelController << " " << origin);
				if (origin == "" || targ->currentHTPLevelController == origin || abs(targ->HTPLevel->floatValue() - newValue) < 0.05) {
					return true;
				}
			}
			if (action == "flashlevel") {
				if (origin == "" || targ->currentFlashLevelController == origin || abs(targ->FlashLevel->floatValue() - newValue) < 0.05) {
					return true;
				}
			}
			if (action == "ltplevel") {
				if (origin == "" || targ->currentLTPLevelController == origin || abs(targ->LTPLevel->floatValue() - newValue) < 0.05) {
					return true;
				}
			}
			if (action == "speed") {
				return true;
			}
		}
		else {return true;}
	}
	else if (targType == "effect") {
		Effect* targ = Brain::getInstance()->getEffectById(targId);
		if (targ != nullptr) {
			String action = effectAction->getValue();
			if (action == "size") {
				if (origin == "" || targ->currentSizeController == origin || abs(targ->sizeValue->floatValue() - newValue) < 0.05) {
					return true;
				}
			}
			if (action == "speed") {
				return true;
			}
		}
		else { return true; }
	}
	else if (targType == "carousel") {
		Carousel* targ = Brain::getInstance()->getCarouselById(targId);
		if (targ != nullptr) {
			String action = carouselAction->getValue();
			if (action == "size") {
				if (origin == "" || targ->currentSizeController == origin || abs(targ->sizeValue->floatValue() - newValue) < 0.05) {
					return true;
				}
			}
			if (action == "speed") {
				return true;
			}
		}
		else { return true; }
	}
	else if (targType == "mapper") {
		Mapper* targ = Brain::getInstance()->getMapperById(targId);
		if (targ != nullptr) {
			String action = mapperAction->getValue();
			if (action == "size") {
				if (origin == "" || targ->currentSizeController == origin || abs(targ->sizeValue->floatValue() - newValue) < 0.05) {
					return true;
				}
			}
		}
		else { return true; }
	}

	return false;
}

