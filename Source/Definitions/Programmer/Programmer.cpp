/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

/*
pour enregistrer, il faut un objet recoder
l'appui sur le bouton rec fait apparaitre le formulaire d'enregistrement dans l'inspecteur
comme pour le moduleManager de chataigne : 		CommunityModuleManager::getInstance()->selectThis();
il faudrait que le formulaire soit pré rempli genre 
- programmer source => id du programmeur 
- type de cible (group, preset, cuelist)
- type d'écriture : remplacer, merge...
- filtre de paramType pour les presets
*/

#include "JuceHeader.h"
#include "Programmer.h"
#include "../../Brain.h"
#include "../ChannelValue.h"

Programmer::Programmer(var params) :
	BaseItem(params.getProperty("name", "Programmer")),
	objectType(params.getProperty("type", "Programmer").toString()),
	objectData(params)
{
	saveAndLoadRecursiveData = true;
	editorIsCollapsed = false;
	itemDataType = "Programmer";
	
	id = addIntParameter("ID", "Id of this Programmer", 0, 0);
	
	editionMode = addEnumParameter("Edition mode", "mode of edition");
	editionMode->addOption("Not timed", "notTimed");
	editionMode->addOption("Timed", "timed");
	editionMode->addOption("Blind", "blind");

	releaseTime = addFloatParameter("Release time", "Fade time when release values",0,0);

	goBtn = addTrigger("GO", "trigger this Programmer");
	releaseBtn = addTrigger("Release", "release this programmer");

	BaseManager<Command>* m = new BaseManager<Command>("Commands");
	m->selectItemWhenCreated = false;
	commands.reset(m);
	addChildControllableContainer(commands.get());

	Brain::getInstance()->registerProgrammer(this, id->getValue());
}

Programmer::~Programmer()
{
	LOG("delete Programmer");
}


void Programmer::triggerTriggered(Trigger* t) {
	if (t == goBtn) {
		go();
	}
	if (t == releaseBtn) {
		release();
	}
}

void Programmer::onContainerParameterChangedInternal(Parameter* p) {
	if (p == id) {
		Brain::getInstance()->registerProgrammer(this, id->getValue());
	}
	if (p == editionMode) {
		if (editionMode->getValue() == "blind") {
			release();
		}
	}
}

void Programmer::computeValues() {
	maxTiming = 0;
	computedValues.clear();
	Array<Command*> cs = commands->getItemsWithType<Command>();
	for (int i = 0; i < cs.size(); i++) {
		cs[i]->computeValues();
		maxTiming = std::max(maxTiming, cs[i]->maxTiming);
		for (auto it = cs[i]->computedValues.begin(); it != cs[i]->computedValues.end(); it.next()) {
			FixtureChannel* fc = it.getKey();
			computedValues.set(fc, it.getValue());
		}
	}
}

void Programmer::go() {
	Brain::getInstance()->pleaseUpdate(this);
}

void Programmer::update(int64 now) {
	render(now);
}

void Programmer::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) {
	LOG (cc->niceName);
	Brain::getInstance()->pleaseUpdate(this);
}

void Programmer::render(int64 now) {
	release(now);
	computeValues();
	String mode = editionMode->getValue();
	if (mode != "blind") {
		for (auto it = computedValues.begin(); it != computedValues.end(); it.next()) {
			ChannelValue* temp = it.getValue();
			if (activeValues.contains(it.getKey())) {
				ChannelValue* current = activeValues.getReference(it.getKey());
				temp->startValue = it.getKey()->value->getValue();
			}
			else {
				temp->startValue = -1;
			}
			temp->TSInit = now;
			if (mode == "timed") {
				temp->TSStart = now + (temp->delay * 1000.0);
				temp->TSEnd = temp->TSStart + (temp->fade * 1000.0);
				temp->isEnded = false;
			}
			else {
				temp->TSStart = now ;
				temp->TSEnd = now;
				temp->isEnded = true;
			}
			activeValues.set(it.getKey(), temp);
			it.getKey()->programmerOnTopOfStack(this);
			Brain::getInstance()->pleaseUpdate(it.getKey());
		}

	}

}

void Programmer::release(int64 now) {
	if (now == 0) {
		now = Time::getMillisecondCounter();
	}
	for (auto it = activeValues.begin(); it != activeValues.end(); it.next()) {
		ChannelValue* temp = it.getValue();
		float fadeTime = releaseTime->getValue();

		temp->TSInit = now;
		temp->TSStart = now;
		temp->TSEnd = now + 1000.0 * fadeTime;

		temp->endValue = -1;
		temp->startValue = temp->value;
		temp->isEnded = false;

		activeValues.set(it.getKey(), temp);
		Brain::getInstance()->pleaseUpdate(it.getKey());
	}

}


float Programmer::applyToChannel(FixtureChannel* fc, float currentVal, int64 now) {
	float val = currentVal;

	bool keepUpdate = false;

	float localValue = 0;
	if (!activeValues.contains(fc)) { return currentVal; }
	ChannelValue* cv = activeValues.getReference(fc);

	float valueFrom = currentVal;
	float valueTo = currentVal;
	float outIsOff = false;
	// comportement OK, calcul de valeurs nok

	if (cv->startValue >= 0) {
		valueFrom = cv->startValue;
	}
	if (cv->endValue >= 0) {
		valueTo = cv->endValue;
	}
	else {
		outIsOff = true;
	}
	if (cv->TSStart > now) {
		localValue = valueFrom;
		keepUpdate = true;
	}
	else if (cv->TSEnd <= now) {
		localValue = valueTo;
		cv->currentPosition = 1;
		if (!cv->isEnded) {
			cv->isEnded = true;
			// Brain::getInstance()->pleaseUpdate(this);
		}
	}
	else {
		float totTime = (cv->delay + cv->fade) * 1000.;
		if (totTime > 0) {
			cv->currentPosition = (now - cv->TSInit) / (totTime);
		}
		else {
			cv->currentPosition = 1;
		}
		float fade = float(now - cv->TSStart) / float(cv->TSEnd - cv->TSStart);
		fade = cv->fadeCurve->getValueAtPosition(fade);
		localValue = jmap(fade, valueFrom, valueTo);
		keepUpdate = true;
		// Brain::getInstance()->pleaseUpdate(this);
	}
	cv->value = localValue;

	if (false) {
		val = jmax(val, localValue);
	}
	else {
		val = localValue;
	}

	if (keepUpdate) {
		Brain::getInstance()->pleaseUpdate(fc);
	}
	return val;
}

