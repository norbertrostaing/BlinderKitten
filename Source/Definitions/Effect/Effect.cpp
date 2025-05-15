/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "Effect.h"
#include "../Command/CommandSelectionManager.h"
#include "../SubFixture/SubFixtureChannel.h"
#include "../../Brain.h"
#include "EffectManager.h"
#include "../CurvePreset/CurvePreset.h"
#include "UI/GridView/EffectGridView.h"
#include "BKEngine.h"

Effect::Effect(var params) :
	BaseItem(params.getProperty("name", "Effect")),
	objectType(params.getProperty("type", "Effect").toString()),
	objectData(params),
	values("Curves"),
	speedMult("Speed multiplicators"),
	sizeMult("Size multiplicators")
{
	saveAndLoadRecursiveData = true;
	nameCanBeChangedByUser = false;
	canBeDisabled = false;

	itemDataType = "Effect";

	id = addIntParameter("ID", "ID of this Effect", 1, 1);
	userName = addStringParameter("Name", "Name of this effect", "New effect");
	layerId = addIntParameter("Layer", "Higher layer, higer priority", 1, 1);
	updateName();

	isEffectOn = addBoolParameter("is ON", "Enable or disable this effect",false);
	isEffectOn->isControllableFeedbackOnly;
	isEffectOn->setEnabled(false);
	isEffectOn->isSavable = false;
	isOn = false;

	startBtn = addTrigger("Start", "Start this effect");
	stopBtn = addTrigger("Stop", "Stop this effect");

	currentPosition = addFloatParameter("Current Position", "Actual position of the effect",0,0,1);
	currentPosition->isControllableFeedbackOnly = true;
	currentPosition->isSavable = false;

	autoStartAndStop = addBoolParameter("Auto Start / Stop", "Start and stop the effect when size is modified", true);
	sizeValue = addFloatParameter("Size", "Master of this Effect", 1, 0, 1);
	flashValue = addFloatParameter("Flash", "Flash master of this Effect", 1, 0, 1);
	speed = addFloatParameter("Speed", "Speed of this effect in cycles/minutes", 5, 0);
	noLoop = addBoolParameter("No loop", "Play this effect only once", false);

	fadeInTime = addFloatParameter("Fade in", "Fade in time in seconds", 0, 0);
	fadeOutTime = addFloatParameter("Fade out", "Fade out time in seconds", 0, 0);

	beatPerCycle = addIntParameter("Beat by cycles", "Number of tap tempo beats by cycle", 1, 1);
	tapTempoBtn = addTrigger("Tap tempo", "Hit me at least twice to se tempo");

	soloPool = addIntParameter("Solo pool", "If greater than zero, only one element can be activated at a time with this number", 0, 0);

	values.selectItemWhenCreated = false;
	addChildControllableContainer(&speedMult);
	addChildControllableContainer(&sizeMult);
	addChildControllableContainer(&values);

	Brain::getInstance()->registerEffect(this, id->getValue());
	if (params.isVoid()) {
		values.addItem();
	}

}

Effect::~Effect()
{
	isComputing.enter();
	Brain::getInstance()->unregisterEffect(this);
	Brain::getInstance()->usingCollections.enter();
	Brain::getInstance()->effectPoolWaiting.removeAllInstancesOf(this);
	Brain::getInstance()->effectPoolUpdating.removeAllInstancesOf(this);
	Brain::getInstance()->usingCollections.exit();
	
	for (auto sfc : Brain::getInstance()->allSubfixtureChannels) {
		if (sfc->effectStack.contains(this)) {
			sfc->effectOutOfStack(this);
			Brain::getInstance()->pleaseUpdate(sfc);
		}
	}
	for (int i = 0; i < values.items.size(); i++) {
		values.items[i]->parentEffect = nullptr;
		for (int j = 0; j < values.items[i]->paramContainer.items.size(); j++) {
			values.items[i]->paramContainer.items[j]->parentEffect = nullptr;

		}
	}
	isComputing.exit();
}

void Effect::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) {
	pleaseComputeIfRunning();
}


void Effect::onContainerParameterChangedInternal(Parameter* p) {
	if (p == id) {
		Brain::getInstance()->registerEffect(this, id->getValue(), true);
	}
	if (p == isEffectOn) {
		Brain::getInstance()->virtualButtonsNeedUpdate = true;
		Brain::getInstance()->virtualFaderButtonsNeedUpdate = true;
		Brain::getInstance()->effectGridNeedRefresh = true;
	}
	if (p == userName || p == id) {
		updateName();
	}
	if (p == sizeValue) {
		if (autoStartAndStop->getValue()) {
			if (isOn && (float)sizeValue->getValue() == 0) {
				kill();
			}
			else if(!isOn && (float)sizeValue->getValue() > 0 && lastSize == 0 && !Engine::mainEngine->isLoadingFile) {
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

void Effect::triggerTriggered(Trigger* t) {
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


void Effect::userStart(bool useFadeIn) {
	userPressedGo = true;
	start(useFadeIn);
}

void Effect::start(bool useFadeIn) {
	TSLastUpdate = Time::getMillisecondCounterHiRes();
	TSEndFadeOut = 0;
	TSStartFadeOut = 0;
	if (useFadeIn && fadeInTime->floatValue()>0) {
		TSStartFadeIn = TSLastUpdate;
		TSEndFadeIn = TSLastUpdate+(fadeInTime->floatValue()*1000);
	}
	else {
		TSStartFadeIn = 0;
		TSEndFadeIn = 0;
	}
	isOn = true;
	isEffectOn->setValue(true);
	totalElapsed = 0;
	computeData();
	if (soloPool->intValue() > 0) Brain::getInstance()->soloPoolEffectStarted(soloPool->intValue(), this);
}

void Effect::stop(float forcedFade) {
	float fade = forcedFade != -1 ? forcedFade : fadeOutTime->floatValue();
	if (fade > 0) {
		TSStartFadeOut = Time::getMillisecondCounterHiRes();
		TSEndFadeOut = TSStartFadeOut + (fade * 1000.0);
	}
	else {
		kill();
	}
}

void Effect::kill() {
	userPressedGo = false;
	isOn = false;
	isEffectOn->setValue(false);
	for (auto it = chanToFxParam.begin(); it != chanToFxParam.end(); it.next()) {
		it.getKey()->effectOutOfStack(this);
		Brain::getInstance()->pleaseUpdate(it.getKey());
	}
	TSStartFadeIn = 0;
	TSStartFadeOut = 0;
	TSEndFadeIn = 0;
	TSEndFadeOut = 0;
}

void Effect::update(double now) {
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
		if (noLoop->boolValue() && totalElapsed > maxOffset && TSEndFadeOut == 0) {
			stop();
		}
	}
	else {
		currentPosition->setValue(0);
	}
}

void Effect::pleaseComputeIfRunning() {
	if (isOn) {
		computed = false;
		Brain::getInstance()->pleaseUpdate(this);
	}
}

void Effect::computeData() {
	//LOG("computing");
	isComputing.enter();
	chanToFxParam.clear();
	maxOffset = 0;
	for (int i = 0; i < values.items.size(); i++) {
		values.items[i]->computeData();
		double rowOffset = values.items[i]->maxOffset;
		rowOffset+=1;
		if (values.items[i]->speed->floatValue() > 0) {
			rowOffset /= values.items[i]->speed->floatValue();
		}
		maxOffset = jmax(maxOffset, rowOffset);
	}
	isComputing.exit();
	computed = true;
	if (isOn) {
		for (auto it = chanToFxParam.begin(); it != chanToFxParam.end(); it.next()) {
			it.getKey()->effectOnTopOfStack(this);
			Brain::getInstance()->pleaseUpdate(it.getKey());
		}
		Brain::getInstance()->pleaseUpdate(this);
	}

}

float Effect::applyToChannel(SubFixtureChannel* fc, float currentVal, double now) {
	isComputing.enter();

	float fadeSize = 1;
	if (TSStartFadeIn < now && TSEndFadeIn > now) fadeSize = jmap(now, TSStartFadeIn, TSEndFadeIn, 0.0, 1.0);
	if (TSStartFadeOut < now && TSEndFadeOut > now) fadeSize = jmap(now, TSStartFadeOut, TSEndFadeOut, 1.0, 0.0);

	if (!chanToFxParam.contains(fc)) { isComputing.exit(); return currentVal; }
	if (isOn) {Brain::getInstance()->pleaseUpdate(fc); }
	std::shared_ptr<Array<EffectParam*>> params = chanToFxParam.getReference(fc);
	for (int i = 0; i < params->size(); i++) {
		EffectParam* p = params->getReference(i);
		p->checkParentEffect();
		EffectRow* row = p->parentEffectRow;
		row->checkParentEffect();

		bool isPerlin = row->curvePresetOrValue->getValue().toString() == "perlin";

		double offset = totalElapsed*(double)row->speed->getValue();
		double deltaOffset = p->subFixtureChannelOffsets.getReference(fc);
		if (!noLoop->boolValue() || row->direction->getValueData() == "bounce") {
			while (offset < 0) {
				offset += 1;
			}
		}
		if (row->direction->getValueData() == "bounce") {
			offset = fmodf(offset, 2);
			if (offset > 1) {
				offset = 2 - offset;
			}
		}
		else if (row->direction->getValueData() == "backward") {
			if (!noLoop->boolValue()) {
				offset = fmodf(offset, 1);
			}
			offset = 1 - offset;
		}
		offset += deltaOffset;
		if (!noLoop->boolValue() && !isPerlin ) {
			while (offset < 0) {
				offset += 1;
			}
			offset = fmodf(offset, 1);
		}
		//

		float value = 0;
		String mode = row->curvePresetOrValue->getValue().toString();
		if (mode == "chaser") {
			float stepSize = 1. / row->selection.computedSelectedSubFixtures.size();
			stepSize *= (float)row->chaserBuddying->getValue();
			float chaseFadeSize = (float)row->chaserFade->getValue();
			chaseFadeSize *= stepSize;
			if (offset < stepSize) {
				value = 1;
				if (offset < chaseFadeSize) {
					float f = offset/chaseFadeSize;
					value = jmap(f, (float)0, (float)1);
				}
			}
			else if (offset < stepSize+chaseFadeSize) {
				float f = (offset-stepSize) / chaseFadeSize;
				value = jmap(f, (float)1, (float)0);
			}
		}
		else if (mode == "perlin") {
			value = row->perlinNoise.octaveNoise0_1(offset, 0, 0, 8);
			value -= (float)row->curveOrigin->getValue();
			if (p->wingsInvertValues->getValue() && p->subFixtureChannelAreWinged.getReference(fc)) {
				value = -value;
			}
			if (p->wingsSoloCenterDisable->getValue() && p->subFixtureChannelAreCentered.getReference(fc)) {
				value = 0;
			}
		}
		else
			{
			Automation* c = &row->curve;
			if (mode == "preset") {
				CurvePreset* pres = Brain::getInstance()->getCurvePresetById(row->presetId->getValue());
				if (pres != nullptr) {
					c = &pres->curve;
				}
			}
			value = c->getValueAtPosition(offset);
			value -= (float)row->curveOrigin->getValue();
			if (p->wingsInvertValues->getValue() && p->subFixtureChannelAreWinged.getReference(fc)) {
				value = -value;
			}
			if (p->wingsSoloCenterDisable->getValue() && p->subFixtureChannelAreCentered.getReference(fc)) {
				value = 0;
			}
		}

		float size = sizeValue->getValue();
		size *= currentSizeMult;
		if (isFlashing) { size = flashValue->floatValue(); }

		size *= fadeSize;

		if (p->effectMode->getValue() == "relative") {
			value *= (double)size;
			value *= (double)p->curveSize->getValue();
			currentVal = currentVal + value;
		}
		else if(p->effectMode->getValue() == "absolute") {
			value *= (double)size;
			value *= (double)p->curveSize->getValue();
			currentVal = (float)p->baseValue->getValue() + value;
		}
		else if(p->effectMode->getValue() == "attracted") {
			value *= (double)p->curveSize->getValue();
			value = (float)p->baseValue->getValue() + value;
			currentVal = jmap((float)size,currentVal,value);
		}


	}
	isComputing.exit();
	return currentVal;
}


void Effect::updateName() {
	String n = userName->getValue();
	if (parentContainer != nullptr) {
		dynamic_cast<EffectManager*>(parentContainer.get())->reorderItems();
	}
	setNiceName(String((int)id->getValue()) + " - " + n);
	Brain::getInstance()->reconstructVirtuals = true;
}


void Effect::tapTempo() {
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

void Effect::flash(bool on, bool swop)
{
	if (on) {
		if (!isOn) {
			start(false);
		}
		isFlashing = true;
		if (swop) {
			isSwopping = true;
			Brain::getInstance()->swoppedEffect(this);
		}
	}
	else {
		isFlashing = false;
		if (isSwopping) {
			isSwopping = false;
			Brain::getInstance()->unswoppedEffect(this);
		}
		if (!userPressedGo) {
			stop();
		}
	}
}
