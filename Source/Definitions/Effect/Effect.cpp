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
#include "../Fixture/FixtureChannel.h"
#include "../../Brain.h"
#include "EffectManager.h"

Effect::Effect(var params) :
	BaseItem(params.getProperty("name", "Effect")),
	objectType(params.getProperty("type", "Effect").toString()),
	objectData(params),
	values("Curves")
{
	saveAndLoadRecursiveData = true;

	itemDataType = "Effect";

	id = addIntParameter("ID", "ID of this Effect", 1, 1);
	userName = addStringParameter("Name", "Name of this effect", "New effect");
	updateName();

	isEffectOn = addBoolParameter("is ON", "Enable or disable this effect",false);
	isEffectOn->isControllableFeedbackOnly;
	isOn = false;

	startBtn = addTrigger("Start", "Start this effect");
	stopBtn = addTrigger("Stop", "Stop this effect");

	currentPosition = addFloatParameter("Current Position", "Actual position of the effect",0,0,1);
	currentPosition->isControllableFeedbackOnly = true;

	sizeValue = addFloatParameter("Size", "Master of this Effect", 1, 0, 1);
	speed = addFloatParameter("Speed", "Speed of this effect in cycles/minutes", 5, 0);
	values.selectItemWhenCreated = false;
	addChildControllableContainer(&values);

	Brain::getInstance()->registerEffect(this, id->getValue());
}

Effect::~Effect()
{
	Brain::getInstance()->unregisterEffect(this);
}

void Effect::onContainerParameterChangedInternal(Parameter* p) {
	if (p == id) {
		Brain::getInstance()->registerEffect(this, id->getValue());
	}
	if (p == userName || p == id) {
		updateName();
	}
}

void Effect::triggerTriggered(Trigger* t) {
	if (t == startBtn) {
		start();
	}
	else if (t == stopBtn) {
		stop();
	}
	else {}
}


void Effect::start() {
	TSLastUpdate = Time::getMillisecondCounterHiRes();
	isOn = true;
	isEffectOn->setValue(true);
	totalElapsed = 0;
	computeData();
	for (auto it = chanToFxParam.begin(); it != chanToFxParam.end(); it.next()) {
		it.getKey()->effectOnTopOfStack(this);
		Brain::getInstance()->pleaseUpdate(it.getKey());
	}
	Brain::getInstance()->pleaseUpdate(this);
}

void Effect::stop() {
	isOn = false;
	isEffectOn->setValue(false);
	for (auto it = chanToFxParam.begin(); it != chanToFxParam.end(); it.next()) {
		it.getKey()->effectOutOfStack(this);
	}
}

void Effect::update(double now) {
	if (isOn) {
		Brain::getInstance()->pleaseUpdate(this);
	}
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

void Effect::computeData() {
	chanToFxParam.clear();
	for (int i = 0; i < values.items.size(); i++) {
		values.items[i]->computeData();
	}
}

float Effect::applyToChannel(FixtureChannel* fc, float currentVal, double now) {
	if (!chanToFxParam.contains(fc)) {return currentVal; }
	if (isOn) {Brain::getInstance()->pleaseUpdate(fc); }
	Array<EffectParam*>* params = chanToFxParam.getReference(fc);
	for (int i = 0; i < params->size(); i++) {
		EffectParam* p = params->getReference(i);
		EffectRow* row = dynamic_cast<EffectRow*>(p->parentContainer->parentContainer.get());

		double offset = totalElapsed*(double)row->speed->getValue();
		offset += p->fixtureChannelOffsets.getReference(fc);
		while (offset < 0) {
			offset += 1;
		}
		offset = fmodf(offset, 1);
		Automation* c = &row->curve;
		float value = c->getValueAtPosition(offset);
		value -= (float)row->curveOrigin->getValue();
		
		if (p->effectMode->getValue() == "relative") {
			value *= (double)sizeValue->getValue();
			value *= (double)p->curveSize->getValue();
			currentVal = currentVal + value;
		}
		else if(p->effectMode->getValue() == "absolute") {
			value *= (double)sizeValue->getValue();
			value *= (double)p->curveSize->getValue();
			currentVal = (float)p->baseValue->getValue() + value;
		}
		else if(p->effectMode->getValue() == "attractive") {
			value *= (double)p->curveSize->getValue();
			value = (float)p->baseValue->getValue() + value;
			currentVal = jmap((float)sizeValue->getValue(),currentVal,value);
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

