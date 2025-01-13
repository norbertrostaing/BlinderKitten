/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "Task.h"
#include "Brain.h"

Task::Task(var params) :
	BaseItem(params.getProperty("name", "Task")),
	objectType(params.getProperty("Task", "Task").toString()),
	objectData(params),
	actionManager("Generic Actions")
{
	saveAndLoadRecursiveData = true;
	
	itemDataType = "Task";
	
	var objectsData = params.getProperty("objects", var());

	targetType = addEnumParameter("Target type", "");
	targetType->addOption("Cuelist", "cuelist");
	targetType->addOption("Effect", "effect");
	targetType->addOption("Carousel", "carousel");
	targetType->addOption("Mapper", "mapper");
	targetType->addOption("Tracker", "tracker");
	targetType->addOption("Bundle", "bundle");
	targetType->addOption("Generic actions", "action");

	targetId = addIntParameter("Target ID", "", 0, 0);
	targetThru = addBoolParameter("target Thru", "multiple targets ?", false);
	targetIdTo = addIntParameter("Target ID To", "", 0, 0);

	cuelistAction = addEnumParameter("Cuelist action", "");
	cuelistAction->addOption("Go", "go");
	cuelistAction->addOption("Off", "off");
	cuelistAction->addOption("Go random", "gorandom");
	cuelistAction->addOption("HTP Level", "htplevel");
	cuelistAction->addOption("Flash Level", "flashlevel");
	cuelistAction->addOption("LTP Level", "ltplevel");
	cuelistAction->addOption("Cancel tasks", "canceltasks");
	cuelistAction->addOption("Set Speed", "speed");

	effectAction = addEnumParameter("Effect action", "");
	effectAction->addOption("Start", "start");
	effectAction->addOption("Stop", "stop");
	effectAction->addOption("Double Speed", "doublespeed");
	effectAction->addOption("Half Speed", "halfspeed");
	effectAction->addOption("Set Size", "size");
	effectAction->addOption("Set Speed", "speed");

	carouselAction = addEnumParameter("Carousel Action", "");
	carouselAction->addOption("Start", "start");
	carouselAction->addOption("Stop", "stop");
	carouselAction->addOption("Toggle", "toggle");
	carouselAction->addOption("Tap tempo", "taptempo");
	carouselAction->addOption("Double Speed", "doublespeed");
	carouselAction->addOption("Half Speed", "halfspeed");
	carouselAction->addOption("Set Size", "size");
	carouselAction->addOption("Set Speed", "speed");

	mapperAction = addEnumParameter("Mapper Action", "");
	mapperAction->addOption("Start", "start");
	mapperAction->addOption("Stop", "stop");
	mapperAction->addOption("Toggle", "toggle");
	mapperAction->addOption("Set Size", "size");

	trackerAction = addEnumParameter("Tracker Action", "");
	trackerAction->addOption("Start", "start");
	trackerAction->addOption("Stop", "stop");
	trackerAction->addOption("Toggle", "toggle");
	trackerAction->addOption("Set Size", "size");

	bundleAction = addEnumParameter("Bundle Action", "");
	bundleAction->addOption("Start", "start");
	bundleAction->addOption("Stop", "stop");
	bundleAction->addOption("Set Size", "size");
	bundleAction->addOption("Set Speed", "speed");

	useHTP = addBoolParameter("Move HTP", "Change size of HTP Level of cuelists", true);
	useLTP = addBoolParameter("Move LTP", "Change size of LTP Level of cuelists", false);
	useSize = addBoolParameter("Move Size", "Change size of effects, carousels, mappers and trackers", true);
	useFlash = addBoolParameter("Move Flash", "change flash levels", false);

	targetValue = addFloatParameter("Target Value", "target value", 0, 0);

	delay = addFloatParameter("Delay", "delay of the first element (in seconds)", 0, 0);
	fade = addFloatParameter("Fade", "fade of the first element (in seconds)", 0, 0);

	addChildControllableContainer(&actionManager);
	updateDisplay();
}

Task::~Task()
{
}

void Task::onContainerParameterChangedInternal(Parameter* c) {
	if (c == targetType  || c == cuelistAction || c == effectAction|| c == carouselAction || c == mapperAction || c == trackerAction || c == bundleAction || c == targetThru) {
		updateDisplay();
	}
	autoName();
}

void Task::updateDisplay() {
	String targType = targetType->getValue();


	targetId->hideInEditor = targType == "action";
	targetThru->hideInEditor = targType == "action" || targType == "bundle";
	targetIdTo->hideInEditor = targType == "action" || targType == "bundle" || !targetThru->getValue();

	cuelistAction->hideInEditor = targType != "cuelist";
	effectAction->hideInEditor = targType != "effect";
	carouselAction->hideInEditor = targType != "carousel";
	mapperAction->hideInEditor = targType != "mapper";
	trackerAction->hideInEditor = targType != "tracker";
	bundleAction->hideInEditor = targType != "bundle";
	actionManager.hideInEditor = targType != "action";

	useHTP->hideInEditor = !(targType == "bundle" && bundleAction->getValue() == "size");
	useLTP->hideInEditor = !(targType == "bundle" && bundleAction->getValue() == "size");
	useSize->hideInEditor = !(targType == "bundle" && bundleAction->getValue() == "size");
	useFlash->hideInEditor = !(targType == "bundle" && bundleAction->getValue() == "size");


	if (targType == "cuelist") {
		if (cuelistAction->getValue() == "htplevel") {
			fade->hideInEditor = false;
			targetValue->hideInEditor = false;
			targetValue->setRange(0, 1);
		}
		else if (cuelistAction->getValue() == "ltplevel") {
			fade->hideInEditor = false;
			targetValue->hideInEditor = false;
			targetValue->setRange(0, 1);
		}
		else if (cuelistAction->getValue() == "flashlevel") {
			fade->hideInEditor = false;
			targetValue->hideInEditor = false;
			targetValue->setRange(0, 1);
		}
		else if (cuelistAction->getValue() == "speed") {
			fade->hideInEditor = false;
			targetValue->hideInEditor = false;
			targetValue->setRange(0, INT32_MAX);
		}
		else {
			fade->hideInEditor = true;
			fade->setValue(0);
			targetValue->hideInEditor = true;
		}
	}
	else if (targType == "effect") {
		if (effectAction->getValue() == "size") {
			fade->hideInEditor = false;
			targetValue->hideInEditor = false;
			targetValue->setRange(0, 1);
		}
		else if (effectAction->getValue() == "speed") {
			fade->hideInEditor = false;
			targetValue->hideInEditor = false;
			targetValue->setRange(0, INT32_MAX);
		}
		else {
			fade->hideInEditor = true;
			fade->setValue(0);
			targetValue->hideInEditor = true;
		}
	}
	else if (targType == "carousel") {
		if (carouselAction->getValue() == "size") {
			fade->hideInEditor = false;
			targetValue->hideInEditor = false;
			targetValue->setRange(0, 1);
		}
		else if (carouselAction->getValue() == "speed") {
			fade->hideInEditor = false;
			targetValue->hideInEditor = false;
			targetValue->setRange(0, INT32_MAX);
		}
		else {
			fade->hideInEditor = true;
			fade->setValue(0);
			targetValue->hideInEditor = true;
		}
	}
	else if (targType == "mapper") {
		if (mapperAction->getValue() == "size") {
			fade->hideInEditor = false;
			targetValue->hideInEditor = false;
			targetValue->setRange(0, 1);
		}
		else {
			fade->hideInEditor = true;
			fade->setValue(0);
			targetValue->hideInEditor = true;
		}
	}
	else if (targType == "tracker") {
		if (trackerAction->getValue() == "size") {
			fade->hideInEditor = false;
			targetValue->hideInEditor = false;
			targetValue->setRange(0, 1);
		}
		else {
			fade->hideInEditor = true;
			fade->setValue(0);
			targetValue->hideInEditor = true;
		}
	}
	else if (targType == "bundle") {
		if (bundleAction->getValue() == "size") {
			fade->hideInEditor = false;
			targetValue->hideInEditor = false;
			targetValue->setRange(0, 1);
		}
		else if (bundleAction->getValue() == "speed") {
			fade->hideInEditor = false;
			targetValue->hideInEditor = false;
			targetValue->setRange(0, 2000);
		}
		else {
			fade->hideInEditor = true;
			fade->setValue(0);
			targetValue->hideInEditor = true;
		}
	}
	else if (targType == "action") {
		fade->hideInEditor = true;
		targetValue->hideInEditor = true;
		
	}

	queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

void Task::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c)
{
	BaseItem::onControllableFeedbackUpdateInternal(cc, c);
	if (!enabled->boolValue()) return;
}


void Task::autoName()
{
	String type = targetType->getValueData();
	if (type == "action") return;
	String name = "";
	name = targetId->stringValue();
	if (targetThru->boolValue()) {
		name += " thru "+targetIdTo->stringValue();
	}
	if (type == "cuelist") { name = cuelistAction->getValueKey() + " cuelist " + name; }
	if (type == "effect") { name = effectAction->getValueKey() + " effect " + name; }
	if (type == "carousel") { name = carouselAction->getValueKey() + " carousel " + name; }
	if (type == "mapper") { name = mapperAction->getValueKey() + " mapper " + name; }
	if (type == "tracker") { name = trackerAction->getValueKey() + " tracker " + name; }
	setNiceName(name);
}

void Task::triggerGivenTask(Task* parentTask, String targetType, int targetId, String action, double value, int id)
{
	if (targetType == "cuelist") {
		Cuelist* target = Brain::getInstance()->getCuelistById(targetId);
		if (target != nullptr) {
			if (action == "go" && value == 1) {
				target->userGo();
			}
			else if (action == "off" && value == 1) {
				target->off();
			}
			else if (action == "gorandom" && value == 1) {
				target->goRandom();
			}
			else if (action == "htplevel") {
				target->setHTPLevel(value);
			}
			else if (action == "ltplevel") {
				target->setLTPLevel(value);
			}
			else if (action == "flashlevel") {
				target->setFlashLevel(value);
			}
			else if (action == "canceltasks") {
				Brain::getInstance()->stopTasks(targetId, id);
			}
			else if (action == "speed") {
				target->chaserSpeed->setValue(value);
			}
		}
	}
	else if (targetType == "effect") {
		Effect* target = Brain::getInstance()->getEffectById(targetId);
		if (target != nullptr) {
			if (action == "start" && value == 1) {
				target->userStart();
			}
			else if (action == "stop" && value == 1) {
				target->stop();
			}
			else if (action == "doublespeed" && value == 1) {
				target->speed->setValue((double)target->speed->getValue() * 2);
			}
			else if (action == "halfspeed" && value == 1) {
				target->speed->setValue((double)target->speed->getValue() / 2);
			}
			else if (action == "size") {
				target->sizeValue->setValue(value);
			}
			else if (action == "speed") {
				target->speed->setValue(value);
			}
		}
	}
	else if (targetType == "carousel") {
		Carousel* target = Brain::getInstance()->getCarouselById(targetId);
		if (target != nullptr) {
			if (action == "start" && value == 1) {
				target->userStart();
			}
			else if (action == "stop" && value == 1) {
				target->stop();
			}
			else if (action == "doublespeed" && value == 1) {
				target->speed->setValue((double)target->speed->getValue() * 2);
			}
			else if (action == "halfspeed" && value == 1) {
				target->speed->setValue((double)target->speed->getValue() / 2);
			}
			else if (action == "size") {
				target->sizeValue->setValue(value);
			}
			else if (action == "speed") {
				target->speed->setValue(value);
			}
		}
	}
	else if (targetType == "mapper") {
		Mapper* target = Brain::getInstance()->getMapperById(targetId);
		if (target != nullptr) {
			if (action == "start" && value == 1) {
				target->start();
			}
			else if (action == "stop" && value == 1) {
				target->stop();
			}
			else if (action == "size") {
				target->sizeValue->setValue(value);
			}
		}
	}
	else if (targetType == "tracker") {
		Tracker* target = Brain::getInstance()->getTrackerById(targetId);
		if (target != nullptr) {
			if (action == "start" && value == 1) {
				target->start();
			}
			else if (action == "stop" && value == 1) {
				target->stop();
			}
			else if (action == "size") {
				target->sizeValue->setValue(value);
			}
		}
	}
	else if (targetType == "action") {
		if (parentTask != nullptr) {
			parentTask->actionManager.setValueAll(1);
		}
	}
	else {
		//LOG("coucou");
	}
}

