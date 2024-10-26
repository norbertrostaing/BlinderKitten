/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "VirtualFaderButton.h"
#include "VirtualFaderColManager.h"
#include "VirtualFaderColGrid.h"
#include "../../Brain.h"
#include "BKEngine.h"
#include "UserInputManager.h"

VirtualFaderButton::VirtualFaderButton(var params) :
	BaseItem(params.getProperty("name", "VirtualFaderButton")),
	objectType(params.getProperty("type", "VirtualFaderButton").toString()),
	objectData(params),
	actionManager("Generic Actions")
{
	saveAndLoadRecursiveData = true;
	nameCanBeChangedByUser = false;
	canBeDisabled = false;

	itemDataType = "VirtualFaderButton";

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

	customText = addStringParameter("Custom Text", "Write your own text on your button", "");

	cuelistAction = addEnumParameter("Cuelist action", "");
	cuelistAction->addOption("Go", "go");
	cuelistAction->addOption("Go back", "goback");
	cuelistAction->addOption("Go instant", "goinstant");
	cuelistAction->addOption("Go back instant", "gobackinstant");
	cuelistAction->addOption("Off", "off");
	cuelistAction->addOption("Toggle", "toggle");
	cuelistAction->addOption("Flash", "flash");
	cuelistAction->addOption("Swop", "swop");
	cuelistAction->addOption("Timed flash", "flashtimed");
	cuelistAction->addOption("Timed swop", "swoptimed");
	cuelistAction->addOption("Load", "load");
	cuelistAction->addOption("Load and Go", "loadandgo");
	cuelistAction->addOption("Go random", "gorandom");

	cueId = addFloatParameter("Cue ID", "Insert here the id of the cue you want to load, -1 will prompt the cue choose window", -1, -1);

	effectAction = addEnumParameter("Effect action", "");
	effectAction->addOption("Start", "start");
	effectAction->addOption("Stop", "stop");
	effectAction->addOption("Toggle", "toggle");
	effectAction->addOption("Flash", "flash");
	effectAction->addOption("Swop", "swop");
	effectAction->addOption("Tap tempo", "taptempo");
	effectAction->addOption("Double Speed", "doublespeed");
	effectAction->addOption("Half Speed", "halfspeed");

	carouselAction = addEnumParameter("Carousel Action", "");
	carouselAction->addOption("Start", "start");
	carouselAction->addOption("Stop", "stop");
	carouselAction->addOption("Toggle", "toggle");
	carouselAction->addOption("Flash", "flash");
	carouselAction->addOption("Swop", "swop");
	carouselAction->addOption("Tap tempo", "taptempo");
	carouselAction->addOption("Double Speed", "doublespeed");
	carouselAction->addOption("Half Speed", "halfspeed");

	mapperAction = addEnumParameter("Mapper Action", "");
	mapperAction->addOption("Start", "start");
	mapperAction->addOption("Stop", "stop");
	mapperAction->addOption("Toggle", "toggle");

	trackerAction = addEnumParameter("Tracker Action", "");
	trackerAction->addOption("Start", "start");
	trackerAction->addOption("Stop", "stop");
	trackerAction->addOption("Toggle", "toggle");

	addChildControllableContainer(&actionManager);

	// id = addIntParameter("ID", "ID of this VirtualFaderButton", 1, 1);
	// userName = addStringParameter("Name", "Name of this VirtualFaderButton","New VirtualFaderButton");
	updateDisplay();
	VirtualFaderColManager::getInstance()->reconstructLibraries();
	VirtualFaderColGrid::getInstance()->fillCells();
}

VirtualFaderButton::~VirtualFaderButton()
{
	VirtualFaderColGrid::getInstance()->buttonToVFB.removeValue(this);
	if (!BKEngine::mainEngine->isClearing) {
		VirtualFaderColManager::getInstance()->reconstructLibraries();
		VirtualFaderColGrid::getInstance()->fillCells();
	}
}

void VirtualFaderButton::updateName() {
}

void VirtualFaderButton::onContainerParameterChangedInternal(Parameter* c) {
	if (c == targetType || c == cuelistAction) {
		updateDisplay();
	}
	VirtualFaderColManager::getInstance()->reconstructLibraries();
	MessageManager::callAsync([this] {VirtualFaderColGrid::getInstance()->fillCells();});
}

void VirtualFaderButton::updateDisplay() {
	String targType = targetType->getValue();
	
	bool isColumn = targType == "column";

	if (targType == "column" && parentContainer != nullptr) {
		targType = dynamic_cast<VirtualFaderCol*>(parentContainer->parentContainer.get())->targetType->getValue();
	}
	cuelistAction->hideInEditor = targType != "cuelist";
	effectAction->hideInEditor = targType != "effect";
	carouselAction->hideInEditor = targType != "carousel";
	mapperAction->hideInEditor = targType != "mapper";
	trackerAction->hideInEditor = targType != "tracker";

	actionManager.hideInEditor = targType != "actions";
	targetId->hideInEditor = isColumn || targType == "disabled" || targType == "actions";

	bool isLoad = targType == "cuelist";
	isLoad = isLoad && (cuelistAction->getValue() == "load" || cuelistAction->getValue() == "loadandgo");

	cueId->hideInEditor = !isLoad;

	queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

void VirtualFaderButton::pressed() {
	checkParentColumn();
	if (parentColumn == nullptr) {LOG("this is strange"); return; }
	String colTargetType = parentColumn->targetType->getValue();
	int colTargetId = parentColumn->targetId->intValue();
	String targType = targetType->getValue();
	if (targType == "actions") {
		actionManager.setValueAll(1);
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
			if (action == "go") { targ->userGo(); }
			if (action == "goback") { targ->goBack(); }
			if (action == "goinstant") { targ->userGo(0,0); }
			if (action == "gobackinstant") { targ->goBack(0,0); }
			if (action == "off") { targ->off(); }
			if (action == "toggle") { targ->toggle(); }
			if (action == "flash") { targ->flash(true, false, false); }
			if (action == "flashtimed") { targ->flash(true, true, false); }
			if (action == "load") {
				float targetCue = cueId->floatValue();
				if (targetCue == -1) {
					targ->showLoad();
				}
				else {
					targ->nextCueId->setValue(targetCue);
				}
			}
			if (action == "loadandgo") {
				float targetCue = cueId->floatValue();
				if (targetCue == -1) {
					targ->showLoadAndGo();
				}
				else {
					targ->nextCueId->setValue(targetCue);
					targ->userGo();
				}
			}
			if (action == "swop") { targ->flash(true, false, true); }
			if (action == "swoptimed") { targ->flash(true, true, true); }
			if (action == "gorandom") { targ->goRandom(); }
		}
	}
	else if (targType == "effect") {
		Effect* targ = Brain::getInstance()->getEffectById(targId);
		if (targ != nullptr) {
			String action = effectAction->getValue();
			if (action == "start") { targ->userStart(); }
			if (action == "stop") { targ->stop(); }
			if (action == "toggle") {
				if (targ->isOn) {
					targ->stop();
				}
				else {
					targ->userStart();
				}
			}
			if (action == "flash") { targ->flash(true); }
			if (action == "swop") { targ->flash(true, true); }
			if (action == "taptempo") { targ->tapTempo(); }
			if (action == "doublespeed") { targ->speed->setValue((double)targ->speed->getValue() * 2); }
			if (action == "halfspeed") { targ->speed->setValue((double)targ->speed->getValue() / 2); }
		}
	}
	else if (targType == "carousel") {
		Carousel* targ = Brain::getInstance()->getCarouselById(targId);
		if (targ != nullptr) {
			String action = carouselAction->getValue();
			if (action == "start") { targ->userStart(); }
			if (action == "stop") { targ->stop(); }
			if (action == "toggle") {
				if (targ->isOn) {
					targ->stop();
				}
				else {
					targ->userStart();
				}
			}
			if (action == "flash") { targ->flash(false); }
			if (action == "swop") { targ->flash(false, true); }
			if (action == "taptempo") { targ->tapTempo(); }
			if (action == "doublespeed") { targ->speed->setValue((double)targ->speed->getValue() * 2); }
			if (action == "halfspeed") { targ->speed->setValue((double)targ->speed->getValue() / 2); }
		}
	}
	else if (targType == "mapper") {
		Mapper* targ = Brain::getInstance()->getMapperById(targId);
		if (targ != nullptr) {
			String action = mapperAction->getValue();
			if (action == "start") { targ->start(); }
			if (action == "stop") { targ->stop(); }
			if (action == "toggle") {
				if (targ->isOn) {
					targ->stop();
				}
				else {
					targ->start();
				}
			}
		}
	}
	else if (targType == "tracker") {
		Tracker* targ = Brain::getInstance()->getTrackerById(targId);
		if (targ != nullptr) {
			String action = trackerAction->getValue();
			if (action == "start") { targ->start(); }
			if (action == "stop") { targ->stop(); }
			if (action == "toggle") {
				if (targ->isOn) {
					targ->stop();
				}
				else {
					targ->start();
				}
			}
		}
	}
}

void VirtualFaderButton::released() {
	checkParentColumn();
	if (parentColumn == nullptr) { LOG("this is strange"); return; }
	String colTargetType = parentColumn->targetType->getValue();
	int colTargetId = parentColumn->targetId->intValue();
	String targType = targetType->getValue();
	if (targType == "actions") {
		actionManager.setValueAll(0);
		return;
	}
	int targId = targetId->getValue();

	if (targType == "column") {
		targType = colTargetType;
		targId = colTargetId;
	}

	if (targId == 0) { return; }

	if (targType == "cuelist") {
		Cuelist* targ = Brain::getInstance()->getCuelistById(targId);
		if (targ != nullptr) {
			String action = cuelistAction->getValue();
			if (action == "flash") { targ->flash(false, false, false); }
			if (action == "swop") { targ->flash(false, false, true); }
			if (action == "flashtimed") { targ->flash(false, true, false); }
			if (action == "swoptimed") { targ->flash(false, true, true); }
		}
	}
	else if (targType == "effect") {
		Effect* targ = Brain::getInstance()->getEffectById(targId);
		if (targ != nullptr) {
			String action = effectAction->getValue();
			if (action == "flash") { targ->flash(false); }
			if (action == "swop") { targ->flash(false, true); }
			// if (action == "start") { targ->start(); }
		}
	}
	else if (targType == "carousel") {
		Carousel* targ = Brain::getInstance()->getCarouselById(targId);
		if (targ != nullptr) {
			String action = carouselAction->getValue();
			if (action == "flash") { targ->flash(false); }
			if (action == "swop") { targ->flash(false, true); }
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

String VirtualFaderButton::getBtnText(String columnType) {
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
				if (!targ->isChaser->boolValue() && targ->cues.items.size() > 1) {
					if (action == "load" || action == "loadandgo") {
						float loadId = cueId->floatValue();
						Cue* c = targ->getCueAfterId(loadId);
						if (c != nullptr && loadId >= 0) {
							text += "\n" + c->niceName;
						}
					}
					else if (targ->cueA != nullptr) {
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

bool VirtualFaderButton::checkParentColumn()
{
	if (parentColumn == nullptr && parentContainer != nullptr) {
		parentColumn = dynamic_cast<VirtualFaderCol*>(parentContainer->parentContainer.get());
	}
	return parentColumn != nullptr;
}


void VirtualFaderButton::updateStatus(bool forceRefresh)
{
	ButtonStatus newStatus = BTN_UNASSIGNED;
	String targType = targetType->getValue();
	int targId = targetId->getValue();

	if (targType == "column") {
		if (!checkParentColumn()) {return;}
		targType = parentColumn->targetType->stringValue();
		targId = parentColumn->targetId->getValue();
	}

	if (targType == "actions") {
		newStatus = BTN_GENERIC;
	}
	else if (targType == "cuelist") {
		Cuelist* targ = Brain::getInstance()->getCuelistById(targId);
		if (targ != nullptr) {
			bool isLoad = targType == "cuelist";
			isLoad = isLoad && (cuelistAction->getValue() == "load" || cuelistAction->getValue() == "loadandgo");
			float loadedCue = -2;

			bool loaded = false;
			if (targ->nextCueId->floatValue() > 0) {
				loaded = true;
				loadedCue = targ->nextCueId->floatValue();
			}
			else if (targ->nextCue->value != "") {
				Cue* nc = dynamic_cast<Cue*>(targ->nextCue->targetContainer.get());
				if (nc != nullptr) {
					loaded = true;
					loadedCue = nc->id->floatValue();
				}
			}

			if (isLoad && loaded && loadedCue == cueId->floatValue())
			{
				newStatus = BTN_LOADEDCUE;
			}
			else if (targ->isCuelistOn->boolValue())
			{
				if (isLoad && targ->cueA != nullptr && targ->cueA->id->floatValue() == cueId->floatValue())
				{
					newStatus = BTN_CURRENTCUE;
				}
				else
				{
					newStatus = loaded ? BTN_ON_LOADED : BTN_ON;
				}
			}
			else
			{
				newStatus = loaded ? BTN_OFF_LOADED : BTN_OFF;
			}
		}
	}
	else if (targType == "effect") {
		Effect* targ = Brain::getInstance()->getEffectById(targId);
		if (targ != nullptr) {
			newStatus = targ->isOn ? BTN_ON : BTN_OFF;
		}
	}
	else if (targType == "carousel") {
		Carousel* targ = Brain::getInstance()->getCarouselById(targId);
		if (targ != nullptr) {
			newStatus = targ->isOn ? BTN_ON : BTN_OFF;
		}
	}
	else if (targType == "mapper") {
		Mapper* targ = Brain::getInstance()->getMapperById(targId);
		if (targ != nullptr) {
			newStatus = targ->isOn ? BTN_ON : BTN_OFF;
		}
	}
	else if (targType == "tracker") {
		Tracker* targ = Brain::getInstance()->getTrackerById(targId);
		if (targ != nullptr) {
			newStatus = targ->isOn ? BTN_ON : BTN_OFF;
		}
	}

	if (currentStatus != newStatus || forceRefresh) {
		feedback(newStatus);
	}

	currentStatus = newStatus;

}


void VirtualFaderButton::feedback(ButtonStatus value)
{
	if (isCurrentlyLoadingData) { return; }
	if (!checkParentColumn()) { return; }

	String address = "";
	String address0 = "";

	int page = parentColumn->pageNumber->intValue();
	int col = parentColumn->colNumber->intValue();
	bool isAbove = true;
	int index = parentColumn->aboveButtons.items.indexOf(this);
	if (index == -1) {
		isAbove = false;
		index = parentColumn->belowButtons.items.indexOf(this);
	}

	String aboveOrBelow = isAbove ? "vabovebutton" : "vbelowbutton";

	address += "/"+aboveOrBelow+"/" + String(page) + "/" + String(col) + "/" + String(index+1);
	address0 += "/"+aboveOrBelow+"/0/" + String(col) + "/" + String(index+1);

	double sentValue = 0;
	sentValue = value;

	UserInputManager::getInstance()->feedback(address, sentValue, "");
	if (page == VirtualFaderColGrid::getInstance()->page) {
		UserInputManager::getInstance()->feedback(address0, sentValue, "");
	}

}

void VirtualFaderButton::feedback(String text)
{
	if (isCurrentlyLoadingData) { return; }
	if (!checkParentColumn()) { return; }

	String address = "";
	String address0 = "";


	int page = parentColumn->pageNumber->intValue();
	int col = parentColumn->colNumber->intValue();
	bool isAbove = true;
	int index = parentColumn->aboveButtons.items.indexOf(this);
	if (index == -1) {
		isAbove = false;
		index = parentColumn->belowButtons.items.indexOf(this);
	}

	String aboveOrBelow = isAbove ? "vabovebutton" : "vbelowbutton";

	address += "/" + aboveOrBelow + "/" + String(page) + "/" + String(col) + "/" + String(index + 1);
	address0 += "/" + aboveOrBelow + "/0/" + String(col) + "/" + String(index + 1);

	UserInputManager::getInstance()->feedback(address, text, "");
	if (page == VirtualFaderColGrid::getInstance()->page) {
		UserInputManager::getInstance()->feedback(address0, text, "");
	}

}
