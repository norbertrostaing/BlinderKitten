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
#include "UI/GridView/CarouselGridView.h"
#include "BKEngine.h"

Carousel::Carousel(var params) :
	BaseItem(params.getProperty("name", "Carousel")),
	objectType(params.getProperty("type", "Carousel").toString()),
	objectData(params),
	rows("Rows"),
	speedMult("Speed multiplicators"),
	sizeMult("Size multiplicators")
{
	saveAndLoadRecursiveData = true;
	nameCanBeChangedByUser = false;
	canBeDisabled = false;

	itemDataType = "Carousel";

	id = addIntParameter("ID", "ID of this Carousel", 1, 1);
	userName = addStringParameter("Name", "Name of this Carousel", "New Carousel");
	layerId = addIntParameter("Layer", "Higher layer, higer priority", 1, 1);
	updateName();

	isCarouselOn = addBoolParameter("is ON", "Enable or disable this Carousel",false);
	isCarouselOn->isControllableFeedbackOnly;
	isCarouselOn->setEnabled(false);
	isCarouselOn->isSavable = false;
	isOn = false;

	startBtn = addTrigger("Start", "Start this Carousel");
	stopBtn = addTrigger("Stop", "Stop this Carousel");

	currentPosition = addFloatParameter("Current Position", "Actual position of the Carousel",0,0,1);
	currentPosition->isControllableFeedbackOnly = true;
	currentPosition->isSavable = false;

	autoStartAndStop = addBoolParameter("Auto Start / Stop", "Start and stop the Carousel when size is modified", true);
	sizeValue = addFloatParameter("Size", "Master of this Carousel", 1, 0, 1);
	flashValue = addFloatParameter("Flash", "Flash master of this Carousel", 1, 0, 1);
	speed = addFloatParameter("Speed", "Speed of this Carousel in cycles/minutes", 5, 0);

	fadeInTime = addFloatParameter("Fade in", "Fade in time in seconds", 0, 0);
	fadeOutTime = addFloatParameter("Fade out", "Fade out time in seconds", 0, 0);

	beatPerCycle = addIntParameter("Beat by cycles", "Number of tap tempo beats by cycle", 1, 1);
	tapTempoBtn = addTrigger("Tap tempo", "Hit me at least twice to se tempo");

	soloPool = addIntParameter("Solo pool", "If greater than zero, only one element can be activated at a time with this number", 0,0);

	addChildControllableContainer(&speedMult);
	addChildControllableContainer(&sizeMult);
	rows.selectItemWhenCreated = false;
	addChildControllableContainer(&rows);

	Brain::getInstance()->registerCarousel(this, id->getValue());

	if (params.isVoid()) {
		rows.addItem();
	}

}

Carousel::~Carousel()
{
	stop();
	isComputing.enter();
	rows.clear();
	isOn = false;
	Brain::getInstance()->unregisterCarousel(this);
	Brain::getInstance()->usingCollections.enter();
	Brain::getInstance()->carouselPoolWaiting.removeAllInstancesOf(this);
	Brain::getInstance()->carouselPoolUpdating.removeAllInstancesOf(this);
	for (SubFixtureChannel* sfc : Brain::getInstance()->allSubfixtureChannels) {
		if (sfc->carouselStack.contains(this)) {
			sfc->carouselOutOfStack(this);
			Brain::getInstance()->pleaseUpdate(sfc);
		}
	}
	Brain::getInstance()->usingCollections.exit();
	for (int i = 0; i < rows.items.size(); i++) {
		rows.items[i]->parentCarousel = nullptr;
		for (int j = 0; j < rows.items[i]->paramContainer.items.size(); j++) {
			rows.items[i]->paramContainer.items[j]->parentCarousel = nullptr;
		}
	}
	isComputing.enter();
}

void Carousel::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) {
	pleaseComputeIfRunning();
}


void Carousel::onContainerParameterChangedInternal(Parameter* p) {
	if (p == id) {
		Brain::getInstance()->registerCarousel(this, id->getValue(), true);
	}
	if (p == isCarouselOn) {
		Brain::getInstance()->virtualButtonsNeedUpdate = true;
		Brain::getInstance()->virtualFaderButtonsNeedUpdate = true;
		Brain::getInstance()->carouselGridNeedRefresh = true;
	}
	if (p == userName || p == id) {
		updateName();
	}
	if (p == sizeValue) {
		if (autoStartAndStop->getValue()) {
			if (isOn && (float)sizeValue->getValue() == 0) {
				kill();
			}
			else if (!isOn && (float)sizeValue->getValue() > 0 && lastSize == 0 && !Engine::mainEngine->isLoadingFile) {
				userStart(false);
			}
		}
		lastSize = p->getValue();
		currentSizeController = nextSizeController;
		nextSizeController = "";
		Brain::getInstance()->virtualFadersNeedUpdate = true;
	}
	if (p == speed) {
		Brain::getInstance()->virtualFadersNeedUpdate = true;
	}
}

void Carousel::triggerTriggered(Trigger* t) {
	if (t == startBtn) {
		userStart();
	}
	else if (t == stopBtn) {
		stop();
	}
	else if (t == tapTempoBtn) {
		tapTempo();
	}
	else {}
}

void Carousel::userStart(bool useFadeIn) {
	userPressedGo = true;
	start(useFadeIn);
}

void Carousel::start(bool useFadeIn) {
	TSLastUpdate = Time::getMillisecondCounterHiRes();
	TSStartFadeOut = 0;
	TSEndFadeOut = 0;
	if (useFadeIn && fadeInTime->floatValue() > 0) {
		TSStartFadeIn = TSLastUpdate;
		TSEndFadeIn = TSLastUpdate + (fadeInTime->floatValue() * 1000);
	}
	else {
		TSStartFadeIn = 0;
		TSEndFadeIn = 0;
	}
	isOn = true;
	isCarouselOn->setValue(true);
	totalElapsed = 0;
	computeData();
	if (soloPool->intValue() > 0) Brain::getInstance()->soloPoolCarouselStarted(soloPool->intValue(), this);
}

void Carousel::stop(float forcedFade) {
	float fade = forcedFade != -1 ? forcedFade : fadeOutTime->floatValue();
	if (fade > 0) {
		TSStartFadeOut = Time::getMillisecondCounterHiRes();
		TSEndFadeOut = TSStartFadeOut + (fade * 1000.0);
	}
	else {
		kill();
	}
}

void Carousel::kill() {
	isOn = false;
	userPressedGo = false;
	isCarouselOn->setValue(false);
	for (auto it = chanToCarouselRow.begin(); it != chanToCarouselRow.end(); it.next()) {
		if (it.getKey() != nullptr) {
			it.getKey()->carouselOutOfStack(this);
		}
	}
	TSStartFadeIn = 0;
	TSStartFadeOut = 0;
	TSEndFadeIn = 0;
	TSEndFadeOut = 0;
}

void Carousel::update(double now) {
	if (computed == false) {
		computeData();
	}
	if (isOn) {
		Brain::getInstance()->pleaseUpdate(this);
		currentSizeMult = sizeMult.getValue();
		double deltaTime = now - TSLastUpdate;
		TSLastUpdate = now;
		double currentSpeed = speed->getValue();
		float speedMultVal = speedMult.getValue();
		currentSpeed *= speedMultVal;
		if (speed != 0) {
			double duration = 60000. / currentSpeed;
			double delta = deltaTime / duration;
			totalElapsed += delta;
			currentPosition->setValue(fmodf(totalElapsed, 1.0));
		}

		if (TSEndFadeOut > 0 && TSEndFadeOut < now) {
			kill();
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
	if (computing) { 
		return; 
	}
	computed = true;
	computing = true;
	isComputing.enter();
	chanToCarouselRow.clear();
	for (int i = 0; i < rows.items.size(); i++) {
		rows.items[i]->computeData();
	}
	computing = false;
	isComputing.exit();
	if (isOn) {
		for (auto it = chanToCarouselRow.begin(); it != chanToCarouselRow.end(); it.next()) {
			if (it.getKey() != nullptr) {
				it.getKey()->carouselOnTopOfStack(this);
				Brain::getInstance()->pleaseUpdate(it.getKey());
			}
		}
		Brain::getInstance()->pleaseUpdate(this);
	}
}

float Carousel::applyToChannel(SubFixtureChannel* fc, float currentVal, double now) {
	if (!chanToCarouselRow.contains(fc)) {return currentVal; }
	if (isOn) {Brain::getInstance()->pleaseUpdate(fc); }

	float fadeSize = 1;
	if (TSStartFadeIn < now && TSEndFadeIn > now) fadeSize = jmap(now, TSStartFadeIn, TSEndFadeIn, 0.0, 1.0);
	if (TSStartFadeOut < now && TSEndFadeOut > now) fadeSize = jmap(now, TSStartFadeOut, TSEndFadeOut, 1.0, 0.0);

	isComputing.enter();
	float calcValue = currentVal;
	bool htpOver = false;
	std::shared_ptr<Array<CarouselRow*>> activeRows = chanToCarouselRow.getReference(fc);
	for (int rId = 0; rId < activeRows->size(); rId++) {
		CarouselRow * r = activeRows->getReference(rId);
		r->checkParentCarousel();
		double offset = totalElapsed;
		offset += r->subFixtureChannelOffsets.getReference(fc);
		while (offset < 0) {
			offset += 1;
		}
		offset = fmodf(offset, 1);
		
		CarouselStep* toApply = nullptr;
		
		for (int stepId = 0; stepId < r->paramContainer.items.size(); stepId++) {
			CarouselStep* step = r->paramContainer.items[stepId];
			step->checkParentCarousel();
			if (step->relativeStartPosition <= offset) {
				toApply = step;
			}	
		}

		if (toApply == nullptr) {
//			LOG("miaou !");
//			LOG(offset);
			isComputing.exit();
			return currentVal;
		}
		std::shared_ptr<ChannelValue> cVal = toApply->computedValues.getReference(fc);
		if (cVal != nullptr) {
			float fadeValue = 1;
			if (cVal->htpOverride) {
				htpOver = true;
			}
			float fadeWidth = toApply->fadeRatio->getValue();
			float pos = (offset-toApply->relativeStartPosition) / toApply->relativeDuration;
			if (pos < fadeWidth) {
				fadeValue = pos/fadeWidth;
				fadeValue = toApply->curve.getValueAtPosition(fadeValue);
			}

			float start = cVal->startValue();
			float end = cVal->endValue();
			start = start == -1 ? currentVal : start;
			end = end == -1 ? currentVal : end;
			calcValue = jmap(fadeValue, start, end);
		}


	}

	float s = sizeValue->getValue();
	s *= currentSizeMult;
	if (isFlashing) { s = flashValue->floatValue(); }
	if (s>1) {s = 1;}

	s *= fadeSize;

	if (fc->isHTP && !htpOver) {
		calcValue *= s;
		currentVal = jmax(calcValue, currentVal);
	}
	else {
		currentVal = jmap(s, currentVal, calcValue);
	}

	isComputing.exit();

	return currentVal;
}


void Carousel::updateName() {
	String n = userName->getValue();
	if (parentContainer != nullptr) {
		dynamic_cast<CarouselManager*>(parentContainer.get())->reorderItems();
	}
	setNiceName(String((int)id->getValue()) + " - " + n);
	Brain::getInstance()->reconstructVirtuals = true;
}

void Carousel::tapTempo() {
	double now = Time::getMillisecondCounterHiRes();
	double delta = now - lastTapTempo;
	lastTapTempo = now;
	if (delta < 3000) {
		BKEngine* e = dynamic_cast<BKEngine*>(BKEngine::mainEngine);
		int historySize = e->tapTempoHistory->intValue();
		delta = delta * beatPerCycle->intValue();
		tapTempoHistory.add(delta);
		while (tapTempoHistory.size() > historySize) tapTempoHistory.remove(0);
		delta = 0;
		for (int i = 0; i < tapTempoHistory.size(); i++) delta += tapTempoHistory[i];
		delta = delta / tapTempoHistory.size();
		double cpm = 60000. / delta;
		speed->setValue(cpm);
	}
	else {
		tapTempoHistory.clear();
	}
}


void Carousel::flash(bool on, bool swop)
{
	if (on) {
		if (!isOn) {
			start(false);
		}
		isFlashing = true;
		if (swop) {
			isSwopping = true;
			Brain::getInstance()->swoppedCarousel(this);
		}
	}
	else {
		isFlashing = false;
		if (isSwopping) {
			isSwopping = false;
			Brain::getInstance()->unswoppedCarousel(this);
		}
		if (!userPressedGo) {
			kill();
		}
	}
}
