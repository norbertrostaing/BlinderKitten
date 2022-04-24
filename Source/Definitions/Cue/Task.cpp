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
	objectData(params)
{
	saveAndLoadRecursiveData = true;
	
	editorIsCollapsed = true;

	itemDataType = "Task";
	
	var objectsData = params.getProperty("objects", var());

	targetType = addEnumParameter("Target type", "");
	targetType->addOption("Cuelist", "cuelist");
	targetType->addOption("Effect", "effect");
	targetType->addOption("Carousel", "carousel");
	targetType->addOption("Mapper", "mapper");

	targetId = addIntParameter("Target ID", "", 0, 0);

	cuelistAction = addEnumParameter("Cuelist action", "");
	cuelistAction->addOption("Go", "go");
	cuelistAction->addOption("Off", "off");
	cuelistAction->addOption("Go random", "gorandom");
	cuelistAction->addOption("HTP Level", "htplevel");
	cuelistAction->addOption("Flash Level", "flashlevel");

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

	targetValue = addFloatParameter("Target Value", "fade of th first element (in seconds)", 0, 0);

	delay = addFloatParameter("Delay", "fade of th first element (in seconds)", 0, 0);
	fade = addFloatParameter("Fade", "fade of th first element (in seconds)", 0, 0);

	updateDisplay();
}

Task::~Task()
{
}

void Task::onContainerParameterChangedInternal(Parameter* c) {
	if (c == targetType  || c == cuelistAction || c == effectAction|| c == carouselAction || c == mapperAction) {
		updateDisplay();
	}
}

void Task::updateDisplay() {
	String targType = targetType->getValue();

	cuelistAction->hideInEditor = targType != "cuelist";
	effectAction->hideInEditor = targType != "effect";
	carouselAction->hideInEditor = targType != "carousel";
	mapperAction->hideInEditor = targType != "mapper";

	if (targType == "cuelist") {
		if (cuelistAction->getValue() == "htplevel") {
			fade->hideInEditor = false;
			targetValue->hideInEditor = false;
			targetValue->setRange(0, 1);
		}
		else if (cuelistAction->getValue() == "flashlevel") {
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

	queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

void Task::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c)
{
	BaseItem::onControllableFeedbackUpdateInternal(cc, c);

	if (!enabled->boolValue()) return;
}


void Task::triggerGivenTask(String targetType, int targetId, String action, double value)
{
	if (targetType == "cuelist") {
		Cuelist* target = Brain::getInstance()->getCuelistById(targetId);
		if (target != nullptr) {
			if (action == "go" && value == 1) {
				target->go();
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
			else if (action == "flashlevel") {
				target->setFlashLevel(value);
			}
		}
	}
	else if (targetType == "effect") {
		Effect* target = Brain::getInstance()->getEffectById(targetId);
		if (target != nullptr) {
			if (action == "start" && value == 1) {
				target->start();
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
				target->start();
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
	else {
		LOG("coucou");

	}
}

