/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "Carousel.h"
#include "../Command/CommandSelectionManager.h"
#include "../SubFixture/SubFixtureChannel.h"
#include "../../Brain.h"
#include "CarouselManager.h"
#include "../CurvePreset/CurvePreset.h"
#include "../ChannelValue.h"

Carousel::Carousel(var params) :
	BaseItem(params.getProperty("name", "Carousel")),
	objectType(params.getProperty("type", "Carousel").toString()),
	objectData(params),
	rows("Rows")
{
	saveAndLoadRecursiveData = true;
	nameCanBeChangedByUser = false;

	itemDataType = "Carousel";

	id = addIntParameter("ID", "ID of this Carousel", 1, 1);
	userName = addStringParameter("Name", "Name of this Carousel", "New Carousel");
	updateName();

	isCarouselOn = addBoolParameter("is ON", "Enable or disable this Carousel",false);
	isCarouselOn->isControllableFeedbackOnly;
	isCarouselOn->setEnabled(false);
	isOn = false;

	startBtn = addTrigger("Start", "Start this Carousel");
	stopBtn = addTrigger("Stop", "Stop this Carousel");

	currentPosition = addFloatParameter("Current Position", "Actual position of the Carousel",0,0,1);
	currentPosition->isControllableFeedbackOnly = true;

	autoStartAndStop = addBoolParameter("Auto Start / Stop", "Start and stop the Carousel when size is modified", false);
	sizeValue = addFloatParameter("Size", "Master of this Carousel", 1, 0, 1);
	speed = addFloatParameter("Speed", "Speed of this Carousel in cycles/minutes", 5, 0);

	beatPerCycle = addIntParameter("Beat by cycles", "Number of tap tempo beats by cycle", 1, 1);
	tapTempoBtn = addTrigger("Tap tempo", "");

	rows.selectItemWhenCreated = false;
	addChildControllableContainer(&rows);

	Brain::getInstance()->registerCarousel(this, id->getValue());

	if (params.isVoid()) {
		rows.addItem();
	}

}

Carousel::~Carousel()
{
	Brain::getInstance()->unregisterCarousel(this);
}

void Carousel::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) {
	pleaseComputeIfRunning();
}


void Carousel::onContainerParameterChangedInternal(Parameter* p) {
	if (p == id) {
		Brain::getInstance()->registerCarousel(this, id->getValue(), true);
	}
	if (p == userName || p == id) {
		updateName();
	}
	if (p == sizeValue) {
		if (autoStartAndStop->getValue()) {
			if (isOn && (float)sizeValue->getValue() == 0) {
				stop();
			}
			else if(!isOn && (float)sizeValue->getValue() > 0) {
				start();
			}
		}
	}
}

void Carousel::triggerTriggered(Trigger* t) {
	if (t == startBtn) {
		start();
	}
	else if (t == stopBtn) {
		stop();
	}
	else if (t == tapTempoBtn) {
		tapTempo();
	}
	else {}
}

void Carousel::start() {
	TSLastUpdate = Time::getMillisecondCounterHiRes();
	isOn = true;
	isCarouselOn->setValue(true);
	totalElapsed = 0;
	computeData();
}

void Carousel::stop() {
	isOn = false;
	isCarouselOn->setValue(false);
	for (auto it = chanToCarouselRow.begin(); it != chanToCarouselRow.end(); it.next()) {
		if (it.getKey() != nullptr) {
			it.getKey()->carouselOutOfStack(this);
		}
	}
}

void Carousel::update(double now) {
	if (computed == false) {
		computeData();
	}
	if (isOn) {
		Brain::getInstance()->pleaseUpdate(this);
		double deltaTime = now - TSLastUpdate;
		TSLastUpdate = now;
		double currentSpeed = speed->getValue();
		if (speed != 0) {
			double duration = 60000. / currentSpeed;
			double delta = deltaTime / duration;
			totalElapsed += delta;
			currentPosition->setValue(fmodf(totalElapsed, 1.0));
		}
	}
	else {
		currentPosition->setValue(0);
	}
}

void Carousel::pleaseComputeIfRunning() {
	if (isOn) {
		computed = false;
		Brain::getInstance()->pleaseUpdate(this);
	}
}

void Carousel::computeData() {
	computed = true;
	computing = true;
	chanToCarouselRow.clear();
	for (int i = 0; i < rows.items.size(); i++) {
		rows.items[i]->computeData();
	}
	if (isOn) {
		for (auto it = chanToCarouselRow.begin(); it != chanToCarouselRow.end(); it.next()) {
			it.getKey()->carouselOnTopOfStack(this);
			Brain::getInstance()->pleaseUpdate(it.getKey());
		}
		Brain::getInstance()->pleaseUpdate(this);
	}
	computing = false;

}

float Carousel::applyToChannel(SubFixtureChannel* fc, float currentVal, double now) {
	if (!chanToCarouselRow.contains(fc)) {return currentVal; }
	if (computing) { return currentVal; }
	if (isOn) {Brain::getInstance()->pleaseUpdate(fc); }
	float calcValue = currentVal;
	Array<CarouselRow*>* activeRows = chanToCarouselRow.getReference(fc);
	for (int rId = 0; rId < activeRows->size(); rId++) {
		CarouselRow * r = activeRows->getReference(rId);
		double offset = totalElapsed;
		offset += r->subFixtureChannelOffsets.getReference(fc);
		while (offset < 0) {
			offset += 1;
		}
		offset = fmodf(offset, 1);
		
		CarouselStep* toApply = nullptr;
		float stepSize = 0;
		for (int stepId = 0; stepId < r->paramContainer.items.size(); stepId++) {
			CarouselStep* step = r->paramContainer.items[stepId];
			if (step->relativeStartPosition <= offset) {
				toApply = step;
			}	
		}

		if (toApply == nullptr) {
			LOG("miaou !");
			LOG(offset);
			return currentVal;
		}
		ChannelValue* cVal = toApply->computedValues.getReference(fc); 
		if (cVal != nullptr) {
			float fadeValue = 1;
			
			float fadeWidth = toApply->fadeRatio->getValue();
			float pos = (offset-toApply->relativeStartPosition) / toApply->relativeDuration;
			if (pos < fadeWidth) {
				fadeValue = pos/fadeWidth;
				fadeValue = toApply->curve.getValueAtPosition(fadeValue);
			}

			float start = cVal->startValue;
			float end = cVal->endValue;
			start = start == -1 ? currentVal : start;
			end = end == -1 ? currentVal : end;
			calcValue = jmap(fadeValue, start, end);
		}


	}

	float s = sizeValue->getValue();
	if (fc->isHTP) {
		calcValue *= s;
		currentVal = jmax(calcValue, currentVal);
	}
	else {
		currentVal = jmap(s, currentVal, calcValue);
	}

	return currentVal;
}


void Carousel::updateName() {
	String n = userName->getValue();
	if (parentContainer != nullptr) {
		dynamic_cast<CarouselManager*>(parentContainer.get())->reorderItems();
	}
	setNiceName(String((int)id->getValue()) + " - " + n);
}

void Carousel::tapTempo() {
	double now = Time::getMillisecondCounterHiRes();
	double delta = now - lastTapTempo;
	lastTapTempo = now;
	if (delta < 3000) {
		delta = delta * (int)beatPerCycle->getValue();
		double cpm = 60000. / delta;
		speed->setValue(cpm);
	}
}