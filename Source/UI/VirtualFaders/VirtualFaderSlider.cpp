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
#include "BKEngine.h"
#include "UserInputManager.h"

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
	targetType->addOption("Tracker", "tracker");
	targetType->addOption("Generic Actions", "actions");

	targetId = addIntParameter("Target ID", "", 0, 0);

	customText = addStringParameter("Custom Text", "Write your own text on your fader", "");

	cuelistAction = addEnumParameter("Cuelist action", "");
	cuelistAction->addOption("HTP Level", "htplevel");
	cuelistAction->addOption("LTP Level", "ltplevel");
	cuelistAction->addOption("Flash Level", "flashlevel");
	cuelistAction->addOption("HTP LTP Levels", "htpltplevel");
	cuelistAction->addOption("Chaser Speed", "speed");
	cuelistAction->addOption("Cross fade", "crossfade");
	cuelistAction->addOption("Up fade", "upfade");
	cuelistAction->addOption("Down fade", "downfade");

	effectAction = addEnumParameter("Effect action", "");
	effectAction->addOption("Size", "size");
	effectAction->addOption("Speed", "speed");

	carouselAction = addEnumParameter("Carousel Action", "");
	carouselAction->addOption("Size", "size");
	carouselAction->addOption("Speed", "speed");

	maxSpeed = addFloatParameter("Max Speed","Speed when your fader is up high",600,0);

	mapperAction = addEnumParameter("Mapper Action", "");
	mapperAction->addOption("Size", "size");

	trackerAction = addEnumParameter("Tracker Action", "");
	trackerAction->addOption("Size", "size");

	addChildControllableContainer(&actionManager);

	updateDisplay();
	updateName();
	VirtualFaderColManager::getInstance()->reconstructLibraries();
	VirtualFaderColGrid::getInstance()->fillCells();

}

VirtualFaderSlider::~VirtualFaderSlider()
{
	VirtualFaderColGrid::getInstance()->sliderToVFS.removeValue(this);
	if (!BKEngine::mainEngine->isClearing) {
		VirtualFaderColManager::getInstance()->reconstructLibraries();
		VirtualFaderColGrid::getInstance()->fillCells();
	}

}

void VirtualFaderSlider::updateName() {
}

void VirtualFaderSlider::onContainerParameterChangedInternal(Parameter* c) {
	if (c == targetType || c == cuelistAction || c == carouselAction || c == effectAction) {
		updateDisplay();
	}
	VirtualFaderColManager::getInstance()->reconstructLibraries();
	MessageManager::callAsync([this] {	VirtualFaderColGrid::getInstance()->fillCells();});
}

void VirtualFaderSlider::updateDisplay() {
	String targType = targetType->getValue();
	bool isColumn = targType == "column";

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
	trackerAction->hideInEditor = targType != "tracker";

	actionManager.hideInEditor = targType != "actions";
	targetId->hideInEditor = isColumn || targType == "disabled" || targType == "actions";

	bool needSpeed = false;
	needSpeed = needSpeed || (targType == "cuelist" && cuelistAction->getValue() == "speed");
	needSpeed = needSpeed || (targType == "effect" && effectAction->getValue() == "speed");
	needSpeed = needSpeed || (targType == "carousel" && carouselAction->getValue() == "speed");

	maxSpeed -> hideInEditor = !needSpeed;

	queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

float VirtualFaderSlider::getTargetValue()
{
	checkParentColumn();
	if (parentColumn == nullptr) { LOG("this is strange"); return 0; }
	String colTargetType = parentColumn->targetType->getValue();
	int colTargetId = parentColumn->targetId->intValue();
	String targType = targetType->getValue();
	int targId = targetId->getValue();

	if (targType == "column") {
		targType = colTargetType;
		targId = colTargetId;
	}

	if (targType == "actions") {
		return actionManager.getValue();
	}

	if (targId == 0) { return 0; }

	if (targType == "cuelist") {
		Cuelist* targ = Brain::getInstance()->getCuelistById(targId);
		if (targ != nullptr) {
			String action = cuelistAction->getValue();
			if (action == "htplevel" || action == "htpltplevel") {
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
			if (action == "crossfade") { return targ->crossFadeController->getValue(); }
			if (action == "upfade") { return targ->upFadeController->getValue(); }
			if (action == "downfade") { return targ->downFadeController->getValue(); }
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
	else if (targType == "tracker") {
		Tracker* targ = Brain::getInstance()->getTrackerById(targId);
		if (targ != nullptr) {
			String action = trackerAction->getValue();
			if (action == "size") { return targ->sizeValue->getValue(); }
		}
	}
	return 0;

}

void VirtualFaderSlider::moved(float value, String origin, bool isRelative) {
	checkParentColumn();
	if (parentColumn == nullptr) { LOG("this is strange"); return; }
	String colTargetType = parentColumn->targetType->getValue();
	int colTargetId = parentColumn->targetId->intValue();
	String targType = targetType->getValue();
	if (!isAllowedToMove(origin, value) && !isRelative) {
		return;
	}

	feedback(value, origin);

	if (targType == "actions") {
		actionManager.setValueAll(value, "VirtualFaders", isRelative);
		return;
	}
	int targId = targetId->getValue();

	if (targType == "column") {
		targType = colTargetType;
		targId = colTargetId;
	}

	if (targId == 0) {return;}
	const MessageManagerLock mmLock;

	if (targType == "cuelist") {
		Cuelist* targ = Brain::getInstance()->getCuelistById(targId);
		if (targ != nullptr) {
			String action = cuelistAction->getValue();
			if (action == "htplevel" || action == "htpltplevel") {
				targ->nextHTPLevelController = origin;
				value = isRelative ? targ->HTPLevel->floatValue()+value : value;
				targ->HTPLevel->setValue(value);
			}
			if (action == "flashlevel") { 
				targ->nextFlashLevelController = origin;
				value = isRelative ? targ->FlashLevel->floatValue() + value : value;
				targ->FlashLevel->setValue(value);
			}
			if (action == "ltplevel" || action == "htpltplevel") {
				targ->nextLTPLevelController = origin;
				value = isRelative ? targ->LTPLevel->floatValue() + value : value;
				targ->LTPLevel->setValue(value);
			}
			if (action == "speed") {
				value = isRelative ? (targ->chaserSpeed->floatValue() / maxSpeed->floatValue()) + value : value;
				targ->chaserSpeed->setValue(value * maxSpeed->floatValue());
			}
			if (action == "crossfade") {
				value = isRelative ? targ->crossFadeController->floatValue() + value : value;
				targ->crossFadeController->setValue(value);
			}
			if (action == "upfade") {
				value = isRelative ? targ->upFadeController->floatValue() + value : value;
				targ->upFadeController->setValue(value);
			}
			if (action == "downfade") {
				value = isRelative ? targ->downFadeController->floatValue() + value : value;
				targ->downFadeController->setValue(value);
			}
		}
	}
	else if (targType == "effect") {
		Effect* targ = Brain::getInstance()->getEffectById(targId);
		if (targ != nullptr) {
			String action = effectAction->getValue();
			if (action == "size") { 
				targ->nextSizeController = origin;
				value = isRelative ? targ->sizeValue->floatValue() + value : value;
				targ->sizeValue->setValue(value);
			}
			if (action == "speed") {
				value = isRelative ? (targ->speed->floatValue() / maxSpeed->floatValue()) + value : value;
				targ->speed->setValue(value * maxSpeed->floatValue());
			}
		}
	}
	else if (targType == "carousel") {
		Carousel* targ = Brain::getInstance()->getCarouselById(targId);
		if (targ != nullptr) {
			String action = carouselAction->getValue();
			if (action == "size") { 
				targ->nextSizeController = origin;
				value = isRelative ? targ->sizeValue->floatValue() + value : value;
				targ->sizeValue->setValue(value);
			}
			if (action == "speed") {
				value = isRelative ? (targ->speed->floatValue() / maxSpeed->floatValue()) + value : value;
				targ->speed->setValue(value * maxSpeed->floatValue());
			}
		}
	}
	else if (targType == "mapper") {
		Mapper* targ = Brain::getInstance()->getMapperById(targId);
		if (targ != nullptr) {
			String action = mapperAction->getValue();
			if (action == "size") {
				targ->nextSizeController = origin;
				value = isRelative ? targ->sizeValue->floatValue() + value : value;
				targ->sizeValue->setValue(value);
			}
		}
	}
	else if (targType == "tracker") {
		Tracker* targ = Brain::getInstance()->getTrackerById(targId);
		if (targ != nullptr) {
			String action = trackerAction->getValue();
			if (action == "size") {
				targ->nextSizeController = origin;
				value = isRelative ? targ->sizeValue->floatValue() + value : value;
				targ->sizeValue->setValue(value);
			}
		}
	}

}

void VirtualFaderSlider::feedback(var value, String origin="")
{
	if (Brain::getInstance()->loadingIsRunning) {return; }
	if (!checkParentColumn()) {return;}
	String address = "";
	String address0 = "";

	int page = parentColumn->pageNumber->intValue();
	int col = parentColumn->colNumber->intValue();

	if (isFader) {
		address += "/vfader/" + String(page) + "/" + String(col);
		address0 += "/vfader/0/" + String(col);
	}
	else {
		int id = parentColumn->rotaries.items.indexOf(this);
		address += "/vrotary/" + String(page) + "/" + String(col) + "/" + String(id + 1);
		address0 += "/vrotary/0/" + String(col) + "/" + String(id + 1);
	}

	UserInputManager::getInstance()->feedback(address, value, origin);
	if (page == VirtualFaderColGrid::getInstance()->page) {
		UserInputManager::getInstance()->feedback(address0, value, origin);
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
	else if (targType == "tracker") {
		Tracker* targ = Brain::getInstance()->getTrackerById(targId);
		if (targ != nullptr) {
			String action = trackerAction->getValue();
			// if (action == "start") { targ->start(); }
		}
	}

}

String VirtualFaderSlider::getBtnText(String columnType) {
	if (customText->stringValue() != "") {
		return customText->stringValue();
	}
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
		else if (targType == "tracker") {
			action = trackerAction->getValue();
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
				if (!targ->isChaser->boolValue() && targ->cues.items.size() > 1 && targ->cueA != nullptr) {
					if (action == "load" || action == "loadandgo") {

					}
					else {
						text += "\n" + targ->cueA->niceName;
					}
				}
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
		else if (targType == "tracker") {
			Tracker* targ = Brain::getInstance()->getTrackerById(targId);
			action = trackerAction->getValue();
			if (targ != nullptr) {
				text = targ->userName->getValue();
			}
		}

		if (text != "") {
			//text = action + "\n" + text;
		}
		return text;
	}
}

bool VirtualFaderSlider::checkParentColumn()
{
	if (parentColumn == nullptr) {
		if (parentContainer != nullptr && parentContainer->parentContainer != nullptr) {
			parentColumn = dynamic_cast<VirtualFaderCol*>(parentContainer->parentContainer.get());
		}
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
			if (action == "htpltplevel") {
				// LOG(targ->currentHTPLevelController << " " << origin);
				if (origin == "" || targ->currentHTPLevelController == origin || abs(targ->HTPLevel->floatValue() - newValue) < 0.05) {
					return true;
				}
				if (origin == "" || targ->currentLTPLevelController == origin || abs(targ->LTPLevel->floatValue() - newValue) < 0.05) {
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
			if (action == "crossfade") {
				if (origin == "" || targ->currentCrossFadeController == origin || abs(targ->crossFadeController->floatValue() - newValue) < 0.05) {
					return true;
				}
			}
			if (action == "upfade") {
				if (origin == "" || targ->currentUpFadeController == origin || abs(targ->upFadeController->floatValue() - newValue) < 0.05) {
					return true;
				}
			}
			if (action == "downfade") {
				if (origin == "" || targ->currentDownFadeController == origin || abs(targ->downFadeController->floatValue() - newValue) < 0.05) {
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
	else if (targType == "tracker") {
		Tracker* targ = Brain::getInstance()->getTrackerById(targId);
		if (targ != nullptr) {
			String action = trackerAction->getValue();
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

