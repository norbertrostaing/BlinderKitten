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
	isOn = false;

	startBtn = addTrigger("Start", "Start this effect");
	stopBtn = addTrigger("Stop", "Stop this effect");

	currentPosition = addFloatParameter("Current Position", "Actual position of the effect",0,0,1);
	currentPosition->isControllableFeedbackOnly = true;

	autoStartAndStop = addBoolParameter("Auto Start / Stop", "Start and stop the effect when size is modified", false);
	sizeValue = addFloatParameter("Size", "Master of this Effect", 1, 0, 1);
	speed = addFloatParameter("Speed", "Speed of this effect in cycles/minutes", 5, 0);

	beatPerCycle = addIntParameter("Beat by cycles", "Number of tap tempo beats by cycle", 1, 1);
	tapTempoBtn = addTrigger("Tap tempo", "");

	values.selectItemWhenCreated = false;
	addChildControllableContainer(&speedMult);
	addChildControllableContainer(&sizeMult);
	addChildControllableContainer(&values);

	Brain::getInstance()->registerEffect(this, id->getValue());
	if (params.isVoid()) {
		values.addItem();
	}
	if (!Brain::getInstance()->loadingIsRunning) {
		EffectGridView::getInstance()->updateCells();
	}

}

Effect::~Effect()
{
	Brain::getInstance()->unregisterEffect(this);
}

void Effect::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) {
	pleaseComputeIfRunning();
}


void Effect::onContainerParameterChangedInternal(Parameter* p) {
	if (p == id) {
		Brain::getInstance()->registerEffect(this, id->getValue(), true);
	}
	if (p == userName || p == id) {
		updateName();
		EffectGridView::getInstance()->updateCells();
	}
	if (p == sizeValue) {
		if (autoStartAndStop->getValue()) {
			if (isOn && (float)sizeValue->getValue() == 0) {
				stop();
			}
			else if(!isOn && (float)sizeValue->getValue() > 0 && lastSize == 0) {
				start();
			}
		}
		lastSize = p->getValue();
		currentSizeController = nextSizeController;
		nextSizeController = "";
	}
}

void Effect::triggerTriggered(Trigger* t) {
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


void Effect::start() {
	TSLastUpdate = Time::getMillisecondCounterHiRes();
	isOn = true;
	isEffectOn->setValue(true);
	totalElapsed = 0;
	computeData();
}

void Effect::stop() {
	isOn = false;
	isEffectOn->setValue(false);
	for (auto it = chanToFxParam.begin(); it != chanToFxParam.end(); it.next()) {
		it.getKey()->effectOutOfStack(this);
	}
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
	computed = true;
	computing = true;
	chanToFxParam.clear();
	for (int i = 0; i < values.items.size(); i++) {
		values.items[i]->computeData();
	}
	if (isOn) {
		for (auto it = chanToFxParam.begin(); it != chanToFxParam.end(); it.next()) {
			it.getKey()->effectOnTopOfStack(this);
			Brain::getInstance()->pleaseUpdate(it.getKey());
		}
		Brain::getInstance()->pleaseUpdate(this);
	}
	computing = false;

}

float Effect::applyToChannel(SubFixtureChannel* fc, float currentVal, double now) {
	if (!chanToFxParam.contains(fc)) {return currentVal; }
	if (computing) { return currentVal; }
	if (isOn) {Brain::getInstance()->pleaseUpdate(fc); }
	Array<EffectParam*>* params = chanToFxParam.getReference(fc);
	for (int i = 0; i < params->size(); i++) {
		EffectParam* p = params->getReference(i);
		EffectRow* row = dynamic_cast<EffectRow*>(p->parentContainer->parentContainer.get());

		double offset = totalElapsed*(double)row->speed->getValue();
		double deltaOffset = p->subFixtureChannelOffsets.getReference(fc);
		while (offset < 0) {
			offset += 1;
		}
		if (row->direction->getValueData() == "bounce") {
			offset = fmodf(offset, 2);
			if (offset > 1) {
				offset = 2 - offset;
			}
		}
		else if (row->direction->getValueData() == "backward") {
			offset = fmodf(offset, 1);
			offset = 1 - offset;
		}
		else {
			offset = fmodf(offset, 1);
		}
		offset += p->subFixtureChannelOffsets.getReference(fc);
		while (offset < 0) {
			offset += 1;
		}
		offset = fmodf(offset, 1);

		float value = 0;
		String mode = row->curvePresetOrValue->getValue().toString();
		if (mode == "chaser") {
			float stepSize = 1. / row->selection.computedSelectedSubFixtures.size();
			stepSize *= (float)row->chaserBuddying->getValue();
			float fadeSize = (float)row->chaserFade->getValue();
			fadeSize *= stepSize;
			if (offset < stepSize) {
				value = 1;
				if (offset < fadeSize) {
					float f = offset/fadeSize;
					value = jmap(f, (float)0, (float)1);
				}
			}
			else if (offset < stepSize+fadeSize) {
				float f = (offset-stepSize) / fadeSize;
				value = jmap(f, (float)1, (float)0);
			}
		}
		else {
			Automation* c = &row->curve;
			if (mode == "preset") {
				CurvePreset* pres = Brain::getInstance()->getCurvePresetById(row->presetId->getValue());
				if (pres != nullptr) {
					c = &pres->curve;
				}
			}
			value = c->getValueAtPosition(offset);
			value -= (float)row->curveOrigin->getValue();
			if (p->wingsInvert->getValue() && p->subFixtureChannelAreWinged.getReference(fc)) {
				value = -value;
			}
		}

		float size = sizeValue->getValue();
		size *= currentSizeMult;

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
		else if(p->effectMode->getValue() == "attractive") {
			value *= (double)p->curveSize->getValue();
			value = (float)p->baseValue->getValue() + value;
			currentVal = jmap((float)size,currentVal,value);
		}


	}
	return currentVal;
}


void Effect::updateName() {
	String n = userName->getValue();
	if (parentContainer != nullptr) {
		dynamic_cast<EffectManager*>(parentContainer.get())->reorderItems();
	}
	setNiceName(String((int)id->getValue()) + " - " + n);
}


void Effect::tapTempo() {
	double now = Time::getMillisecondCounterHiRes();
	double delta = now - lastTapTempo;
	lastTapTempo = now;
	if (delta < 3000) {
		delta = delta * (int)beatPerCycle->getValue();
		double cpm = 60000. / delta;
		speed->setValue(cpm);
	}
}