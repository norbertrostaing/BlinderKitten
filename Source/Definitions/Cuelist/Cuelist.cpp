/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "Cuelist.h"
#include "Brain.h"
#include "../ChannelValue.h"

int sortCues(Cue* A, Cue* B) {
	String test = A->id->getValue() > B->id->getValue() ? "y" : "n";
	return A->id->getValue() > B->id->getValue() ? 1 : -1;
}

Cuelist::Cuelist(var params) :
	BaseItem(params.getProperty("name", "Cuelist")),
	objectType(params.getProperty("type", "Cuelist").toString()),
	objectData(params),
	offFadeCurve()
{
	saveAndLoadRecursiveData = true;
	editorIsCollapsed = true;
	itemDataType = "Cuelist";

	CueManager* m = new CueManager();
	m->selectItemWhenCreated = false;
	cues.reset(m);

	id = addIntParameter("ID", "Id of this Cuelist", 1, 1);

	endAction = addEnumParameter("Loop", "Behaviour of this cuelist at the end of its cues");
	endAction->addOption("Off", "off");
	endAction->addOption("Loop", "loop");

	offIfOverwritten = addBoolParameter("Off If Overwritten", "If checked, this cuelist will kill itself if all parameter are overwritten by other cuelists", false);

	tracking = addEnumParameter("Tracking", "Tracking will keep the values of previous cues enabled");
	tracking->addOption("None", "none");
	tracking->addOption("Cuelist order", "cuelist");
	tracking->addOption("Call order", "call");

	loopTracking = addBoolParameter("Loop Tracking", "Do you want tracking keeps values when coming back to start after loop ?", false);

	goBtn = addTrigger("GO", "Trigger next cue");

	nextCue = addTargetParameter("Next Cue", "Cue triggered when button go pressed", cues.get());
	nextCue->maxDefaultSearchLevel = 0;
	nextCue->targetType = TargetParameter::CONTAINER;

	nextCueId = addFloatParameter("Next cue ID", "ID of the cue triggered when go pressed, 0 means next cue",0,0);

	offBtn = addTrigger("OFF", "Off this cuelist");
	killBtn = addTrigger("KILL", "Kill this cuelist and leave no clues");

	HTPLevel = addFloatParameter("HTP Level", "Level master for HTP channels of this sequence", 1, 0, 1);
	// LTPLevel = addFloatParameter("LTP Level", "Level master for LTP channels of this sequence", 1, 0, 1);

	isCuelistOn = addBoolParameter("is On", "Is this cuelist on ?", false);
	isCuelistOn->isControllableFeedbackOnly = true;

	currentFade = addFloatParameter("Current Xfade", "Current position of the running cue", 0, 0, 1);
	currentFade->isControllableFeedbackOnly = true;

	offFade = addFloatParameter("Off time", "Default fade time used to off the cuelist", 0, 0);

	// offFadeCurve = new Automation();
	offFadeCurve.saveAndLoadRecursiveData = true;
	offFadeCurve.setNiceName("Off curve");
	offFadeCurve.allowKeysOutside = false;
	offFadeCurve.isSelectable = false;
	offFadeCurve.length->setValue(1);
	offFadeCurve.addKey(0, 0, false);
	offFadeCurve.items[0]->easingType->setValueWithData(Easing::LINEAR);
	offFadeCurve.addKey(1, 1, false);
	offFadeCurve.selectItemWhenCreated = false;
	offFadeCurve.editorCanBeCollapsed = true;
	addChildControllableContainer(&offFadeCurve);

	//nextCueId = addFloatParameter("Next Cue", "Index of the next cue", - 1, -1);
	cueA = nullptr; // static current cue
	cueB = nullptr; // loaded Cue

	// currentCue = addTargetParameter("Current Cue", "Last cue triggered", cues.get());
	// currentCue->maxDefaultSearchLevel = 0;
	// currentCue->targetType = TargetParameter::CONTAINER;


	addChildControllableContainer(cues.get());

	cues->comparator.compareFunc = &sortCues;
	reorderCues();

	Brain::getInstance()->registerCuelist(this, id->getValue());
}

Cuelist::~Cuelist()
{
	Brain::getInstance()->unregisterCuelist(this);
}

void Cuelist::reorderCues() {
	cues->reorderItems();
	//cues->queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, cues.get()));
}

void Cuelist::onContainerParameterChangedInternal(Parameter* p) {
	if (p == HTPLevel) {
		updateHTPs();
	}
	if (p == id) {
		Brain::getInstance()->registerCuelist(this, id->getValue());
	}
	//if (p == LTPLevel) {
		// updateLTPs();
	//}
}

void Cuelist::triggerTriggered(Trigger* t) {
	if (t == goBtn) {
		go();
	}
	else if(t == offBtn) {
		off();
	}
	else if(t == killBtn) {
		kill();
	}
	else {}
}

void Cuelist::go(Cue* c) {
	double now = Time::getMillisecondCounterHiRes();;
	if (cueA != nullptr) {
		cueA->TSAutoFollowEnd = 0;
	}
	cueA = c;
	LOG("go Cuelist");
	cueB = nullptr;
	nextCue->resetValue();
	nextCueId->resetValue();
	if (c != nullptr) {
		isCuelistOn->setValue(true);
	}

	String trackingType = tracking->getValue();
	if (trackingType == "none" || c == nullptr) {
		for (auto it = activeValues.begin(); it != activeValues.end(); it.next()) {
			ChannelValue* temp = it.getValue();
			float fadeTime = (float)offFade->getValue()*1000;
			temp->fadeCurve = &offFadeCurve;

			temp->TSInit = now;
			temp->TSStart = now;
			temp->TSEnd= now + fadeTime; 

			temp->endValue = -1;
			temp->startValue = temp->value;
			temp->isEnded = false;

			activeValues.set(it.getKey(), temp);
			Brain::getInstance()->pleaseUpdate(it.getKey());
		}
	}

	if (c != nullptr) {
		c->computeValues();
		for (auto it = c->computedValues.begin(); it != c->computedValues.end(); it.next()) {
			ChannelValue* temp = it.getValue();
			if (activeValues.contains(it.getKey())) {
				ChannelValue * current = activeValues.getReference(it.getKey());
				temp->startValue = it.getKey()->value->getValue();
			}
			else {
				temp->startValue = -1;
			}
			temp->TSInit = now;
			temp->TSStart = now + (temp->delay);
			temp->TSEnd = temp -> TSStart + (temp->fade );
			temp -> isEnded = false;
			activeValues.set(it.getKey(), temp);
			it.getKey()->cuelistOnTopOfStack(this);
			Brain::getInstance()->pleaseUpdate(it.getKey());
		}
		c->go();
	}

	return ;
	// old version
	/*
	if (!isOn && c!= nullptr) {
		isOn = true;
		//Brain::getInstance()->cuelistOnTopOfStack(this);
	}

	if (TSTransitionEnd > 0) {
		endTransition();
		return;
	}

	TSTransitionStart = Time::currentTimeMillis();
	TSTransitionDuration = 1;
	currentTimeElapsed = 0;

	String trackingType = tracking->getValue();
	if (trackingType == "none" || c == nullptr) {
		for (auto it = activeValues.begin(); it != activeValues.end(); it.next()) {
			ChannelValue* temp = new ChannelValue();
			temp->fade = offFade->getValue();
			temp->fadeCurve = offFadeCurve;
			temp->value = -1;
			fadingValues.set(it.getKey(), temp);
			Brain::getInstance()->pleaseUpdate(it.getKey());
		}
	}

	if (c != nullptr) {
		c->computeValues();
		for (auto it = c->computedValues.begin(); it != c->computedValues.end(); it.next()) {
			fadingValues.set(it.getKey(), it.getValue());
			Brain::getInstance()->pleaseUpdate(it.getKey());
		}
	}

	for (auto it = fadingValues.begin(); it != fadingValues.end(); it.next()) {
		int64 delay = it.getValue() -> delay * 1000;
		int64 fade = it.getValue() -> fade * 1000;
		TSTransitionDuration = jmax(TSTransitionDuration, delay + fade);
	}
	TSTransitionEnd = TSTransitionStart + TSTransitionDuration;
	Brain::getInstance()->pleaseUpdate(this);
	*/
}

void Cuelist::go() {
	cueB = dynamic_cast<Cue*>(nextCue->targetContainer.get());
	float nextId = nextCueId->getValue();
	if (nextId > 0) {
		for (int i = 0; i < cues->items.size() && cueB == nullptr; i++) {
			if ((float)cues->items[i]->id->getValue() >= nextId) {
				cueB = cues->items[i];
			}
		}
	}
	if (cueB == nullptr) {
		autoLoadCueB();
	}
	go(cueB);
}

void Cuelist::off() {
	go(nullptr);
}



void Cuelist::update() {
	float tempPosition = 1;
	float isUseFul = false;
	float isOverWritten = true;
	for (auto it = activeValues.begin(); it != activeValues.end(); it.next()) {
		ChannelValue* cv = it.getValue();
		tempPosition = jmin(tempPosition, cv->currentPosition);
		isUseFul = isUseFul || cv -> endValue != -1 || !cv->isEnded;
		isOverWritten = isOverWritten && cv -> isOverWritten;
	}
	currentFade->setValue(tempPosition);
	if (tempPosition == 1 && cueA != nullptr) {
		cueA->endTransition();
	}
	if (isOverWritten && offIfOverwritten->getValue()) {
		isUseFul = false;
	}
	if (!isUseFul) {
		kill(false);
	}
}

void Cuelist::autoLoadCueB() {
	bool valid = false;
	if (cueA == nullptr) {
		if (cues->getItemsWithType<Cue>().size() > 0) {
			cueB = cues->getItemsWithType<Cue>()[0];
			valid = true;
		}
	}
	else {
		Array<Cue*> currentCues = cues->getItemsWithType<Cue>();
		for (int i = 1; i < currentCues.size() && !valid; i++) {
			if (currentCues[i - 1] == cueA) {
				valid = true;
				cueB = currentCues[i];
			}
		}
		if (!valid) { // cueA is the last cue
			String end = endAction->getValue();
			if (end == "loop") {
				cueB = currentCues[0];
			}
			else {
				cueB = nullptr;
			}
		}
	}
}

float Cuelist::applyToChannel(FixtureChannel* fc, float currentVal, double now) {
	float val = currentVal;
	bool HTP = fc->parentParamDefinition->priority->getValue() == "HTP";
	
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
		if (HTP) {
			valueTo *= (float)HTPLevel->getValue();
		}
	}
	else {
		outIsOff= true;
	}
	if (cv -> TSStart > now) {
		localValue = valueFrom; 
		keepUpdate = true;
	}
	else if (cv -> TSEnd <= now) {
		localValue = valueTo;
		cv->currentPosition = 1;
		if (!cv->isEnded) {
			cv->isEnded = true;
			Brain::getInstance()->pleaseUpdate(this);
		}
	}
	else {
		float totTime = (cv->delay + cv->fade);
		if (totTime > 0) {
			cv->currentPosition = (now - cv->TSInit) / (totTime);
		}
		else {
			cv->currentPosition = 1;
		}
		float fade = double(now - cv->TSStart) / double(cv -> TSEnd - cv->TSStart);
		fade = cv->fadeCurve->getValueAtPosition(fade);
		localValue = jmap(fade, valueFrom, valueTo);
		keepUpdate = true;
		Brain::getInstance()->pleaseUpdate(this);
	}
	cv->value = localValue;

	if (HTP) {
		val = jmax(val, localValue);
	}
	else {
		// float ratio = LTPLevel->getValue();
		// val = jmap(ratio,valueFrom,localValue);
		val = localValue;
	}

	if (keepUpdate) {
		Brain::getInstance()->pleaseUpdate(fc);
	}
	return val;
	// old version
	/*
	float valA = -1;
	float valB = -1;
	float ratioB = 0;

	if (activeValues.contains(fc)) {
		valA = activeValues.getReference(fc)->value;
		ratioB = 0;
	}
	if (fadingValues.contains(fc)) {
		ChannelValue* cv = fadingValues.getReference(fc);
		valB = cv->value;
		float fade = cv->fade * 1000;
		float delay = cv->delay * 1000;
		if (currentTimeElapsed < delay) {
			ratioB = 0;
		}
		else {
			ratioB = (currentTimeElapsed - delay) / fade;
		}
		if (ratioB >= 1) {
			activeValues.set(fc, cv);
			fadingValues.remove(fc);
			ratioB = 1;
		}
		else {
			ratioB = cv->fadeCurve->getValueAtPosition(ratioB);
			Brain::getInstance()->pleaseUpdate(fc);
		}
	}


	if (HTP) {
		valA = valA == -1 ? 0 : valA;
		valB = valB == -1 ? 0 : valB;
		float valCuelist = jmap(ratioB,valA, valB);
		float level = HTPLevel->getValue();
		valCuelist *= level;
		return jmax(valCuelist, currentVal);
	}
	else { // LTP
		// LOG("LTP");
		float endValue = currentVal;
		float level = LTPLevel->getValue();
		if (valA != -1 && valB != -1) {
			// LOG("n1 "+fc->niceName);
			// LOG(ratioB);
			// LOG(valA);
			// LOG(valB);
			float valCuelist = jmap(ratioB, valA, valB);
			endValue = jmap(level,currentVal, valCuelist);
		}
		else if (valB != -1) {
			// LOG("2");
			endValue = jmap(level*ratioB, currentVal, valB);
		}
		else if (valA != -1) {
			// LOG("3");
			endValue = jmap(ratioB, valA, currentVal);
			endValue = jmap(level, currentVal, endValue);
		}
		return endValue;
	}

	return 0;
	*/
}

void Cuelist::cleanActiveValues() {
	Array<FixtureChannel*> temp;
	for (auto it = activeValues.begin(); it != activeValues.end(); it.next()) {
		if (it.getValue()->value == -1) {
			temp.add(it.getKey());
		}
	}
	for (int i = 0; i < temp.size(); i++) {
		activeValues.remove(temp[i]);
	}
}

void Cuelist::updateHTPs() {
	for (auto it = activeValues.begin(); it != activeValues.end(); it.next()) {
		FixtureChannel* chan = it.getKey();
		if (chan->isHTP) {
			Brain::getInstance()->pleaseUpdate(chan);
		}
	}
}

void Cuelist::updateLTPs() {
	for (auto it = activeValues.begin(); it != activeValues.end(); it.next()) {
		FixtureChannel* chan = it.getKey();
		if (!chan->isHTP) {
			Brain::getInstance()->pleaseUpdate(chan);
		}
	}
}


void Cuelist::kill(bool forceRefreshChannels) {
	for (auto it = activeValues.begin(); it != activeValues.end(); it.next()) {
		FixtureChannel* chan = it.getKey();
		chan->cuelistOutOfStack(this);
		if (forceRefreshChannels) {
			Brain::getInstance()->pleaseUpdate(chan);
		}
	}
	cueA = nullptr;
	cueB = nullptr;
	isCuelistOn->setValue(false);
}

void Cuelist::setHTPLevel(float level) {
	HTPLevel->setValue(level);
}

