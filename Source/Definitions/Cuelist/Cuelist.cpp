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
#include "CuelistManager.h"

int sortCues(Cue* A, Cue* B) {
	String test = A->id->getValue() > B->id->getValue() ? "y" : "n";
	return A->id->getValue() > B->id->getValue() ? 1 : -1;
}

Cuelist::Cuelist(var params) :
	BaseItem(params.getProperty("name", "Cuelist")),
	objectType(params.getProperty("type", "Cuelist").toString()),
	objectData(params),
	offFadeCurve(),
	cues()
{
	saveAndLoadRecursiveData = true;
	nameCanBeChangedByUser = false;
	editorIsCollapsed = true;
	itemDataType = "Cuelist";

	id = addIntParameter("ID", "Id of this Cuelist", 1, 1);
	userName = addStringParameter("Name", "Name of this cuelist", "New cuelist");
	updateName();

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
	goRandomBtn = addTrigger("GO random", "Trigger a random cue");
	offBtn = addTrigger("OFF", "Off this cuelist");
	killBtn = addTrigger("KILL", "Kill this cuelist and leave no clues");
	flashOnBtn = addTrigger("Flash ON", "Trigger next cue");
	flashOffBtn = addTrigger("flash Off", "Trigger next cue");

	nextCue = addTargetParameter("Next Cue", "Cue triggered when button go pressed", &cues);
	nextCue->maxDefaultSearchLevel = 0;
	nextCue->targetType = TargetParameter::CONTAINER;

	nextCueId = addFloatParameter("Next cue ID", "ID of the cue triggered when go pressed, 0 means next cue",0,0);

	HTPLevel = addFloatParameter("HTP Level", "Level master for HTP channels of this sequence", 1, 0, 1);
	FlashLevel = addFloatParameter("Flash Level", "Level flash master for HTP channels of this sequence", 1, 0, 1);
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


	addChildControllableContainer(&cues);

	cues.comparator.compareFunc = &sortCues;
	reorderCues();

	if (params.isVoid()) {
		cues.addItem();
	}

	Brain::getInstance()->registerCuelist(this, id->getValue());
}

Cuelist::~Cuelist()
{
	Brain::getInstance()->unregisterCuelist(this);
}

void Cuelist::reorderCues() {
	cues.reorderItems();
	//cues->queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, cues.get()));
}

void Cuelist::onContainerParameterChangedInternal(Parameter* p) {
	if (p == HTPLevel || p == FlashLevel) {
		pleaseUpdateHTPs = true;
		Brain::getInstance()->pleaseUpdate(this);
	}
	if (p == id) {
		Brain::getInstance()->registerCuelist(this, id->getValue());
	}
	if (p == userName || p == id) {
		updateName();
	}
}

void Cuelist::triggerTriggered(Trigger* t) {
	if (t == goBtn) {
		go();
	}
	else if (t == goRandomBtn) {
		goRandom();
	}
	else if(t == offBtn) {
		off();
	}
	else if (t == killBtn) {
		kill();
	}
	else if (t == flashOnBtn) {
		flash(true, true);
	}
	else if (t == flashOffBtn) {
		flash(false, true);
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
				temp->startValue = it.getKey()->postCuelistValue;
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
}

void Cuelist::go() {
	cueB = dynamic_cast<Cue*>(nextCue->targetContainer.get());
	float nextId = nextCueId->getValue();
	if (nextId > 0) {
		for (int i = 0; i < cues.items.size() && cueB == nullptr; i++) {
			if ((float)cues.items[i]->id->getValue() >= nextId) {
				cueB = cues.items[i];
			}
		}
	}
	if (cueB == nullptr) {
		autoLoadCueB();
	}
	go(cueB);
}

void Cuelist::flash(bool setOn, bool withTiming) {
	if (setOn) {
		isFlashing = true;
		double now = Time::getMillisecondCounterHiRes();

		Cue* flashingCue = cueA;
		if (flashingCue == nullptr) {
			autoLoadCueB();
			flashingCue = cueB;
		}

		LOG("go Flash");
		cueB = nullptr;

		if (flashingCue != nullptr) {
			LOG("ok");
			// set a flash tag to on ?
			flashingCue->computeValues();
			for (auto it = flashingCue->computedValues.begin(); it != flashingCue->computedValues.end(); it.next()) {
				ChannelValue* temp = it.getValue();
				if (flashingValues.contains(it.getKey())) {
					ChannelValue* current = flashingValues.getReference(it.getKey());
					temp->startValue = it.getKey()->value;
				}
				else {
					temp->startValue = -1;
				}
				temp->TSInit = now;
				temp->TSStart = withTiming ? now + (temp->delay) : now;
				temp->TSEnd = withTiming ? temp->TSStart + (temp->fade) : now;
				temp->isEnded = false;
				flashingValues.set(it.getKey(), temp);
				LOG("yataaaaa");
				it.getKey()->cuelistOnTopOfFlashStack(this);
				Brain::getInstance()->pleaseUpdate(it.getKey());
			}
		}
	}
	else {
		wannaOffFlash = true;
		for (auto it = flashingValues.begin(); it != flashingValues.end(); it.next()) {
			Brain::getInstance()->pleaseUpdate(it.getKey());
		}
		Brain::getInstance()->pleaseUpdate(this);
	}
}


void Cuelist::goRandom() {
	Array<Cue*> childCues = cues.getItemsWithType<Cue>();
	Array<Cue*> allowedCues;

	for (int i = 0; i< childCues.size(); i++) {
		if (childCues[i]!=cueA && childCues[i]->canBeRandomlyCalled->getValue()) {
			allowedCues.add(childCues[i]);
		}
	}

	int s = allowedCues.size();
	if (s > 0) {
		int r = rand()%s;
		go(allowedCues[r]);
	}
}

void Cuelist::off() {
	go(nullptr);
}








void Cuelist::update() {
	if (pleaseUpdateHTPs) {
		updateHTPs();
	}
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

	if (wannaOffFlash) {
		LOG("wannaOff");
		bool canStopFlash = true;
		for (auto it = flashingValues.begin(); it != flashingValues.end(); it.next()) {
			ChannelValue* cv = it.getValue();
			canStopFlash = canStopFlash && cv->isEnded;
		}

		if (canStopFlash) {
			LOG("yes");
			for (auto it = flashingValues.begin(); it != flashingValues.end(); it.next()) {
				it.getKey()->cuelistOutOfFlashStack(this);
				Brain::getInstance()->pleaseUpdate(it.getKey());
			}
			isFlashing = false;
			wannaOffFlash = false;
		}
	}
}

void Cuelist::autoLoadCueB() {
	bool valid = false;
	if (cueA == nullptr) {
		if (cues.getItemsWithType<Cue>().size() > 0) {
			cueB = cues.getItemsWithType<Cue>()[0];
			valid = true;
		}
	}
	else {
		Array<Cue*> currentCues = cues.getItemsWithType<Cue>();
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

float Cuelist::applyToChannel(SubFixtureChannel* fc, float currentVal, double now, bool flashvalues) {
	float val = currentVal;
	bool HTP = fc->parentParamDefinition->priority->getValue() == "HTP";
	
	bool keepUpdate = false;

	float localValue = 0;
	ChannelValue* cv;
	float faderLevel = 0;
	if (flashvalues) {
		if (!flashingValues.contains(fc)) { return currentVal; }
		cv = flashingValues.getReference(fc);
		faderLevel = (float)FlashLevel->getValue();
	}
	else {
		if (!activeValues.contains(fc)) { return currentVal; }
		cv = activeValues.getReference(fc);
		faderLevel = (float)HTPLevel->getValue();
	}

	if (cv == nullptr) {
		LOG("c'est chelou");
		return currentVal; 
	}

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
			valueTo *= faderLevel;
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
}

void Cuelist::cleanActiveValues() {
	Array<SubFixtureChannel*> temp;
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
	pleaseUpdateHTPs = false;
	for (auto it = activeValues.begin(); it != activeValues.end(); it.next()) {
		SubFixtureChannel* chan = it.getKey();
		if (chan->isHTP) {
			Brain::getInstance()->pleaseUpdate(chan);
		}
	}
	for (auto it = flashingValues.begin(); it != flashingValues.end(); it.next()) {
		SubFixtureChannel* chan = it.getKey();
		if (chan->isHTP) {
			Brain::getInstance()->pleaseUpdate(chan);
		}
	}
}

void Cuelist::updateLTPs() {
	for (auto it = activeValues.begin(); it != activeValues.end(); it.next()) {
		SubFixtureChannel* chan = it.getKey();
		if (!chan->isHTP) {
			Brain::getInstance()->pleaseUpdate(chan);
		}
	}
}


void Cuelist::kill(bool forceRefreshChannels) {
	for (auto it = activeValues.begin(); it != activeValues.end(); it.next()) {
		SubFixtureChannel* chan = it.getKey();
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

void Cuelist::setFlashLevel(float level) {
	FlashLevel->setValue(level);
}


void Cuelist::updateName() {
	String n = userName->getValue();
	if (parentContainer != nullptr) {
		dynamic_cast<CuelistManager*>(parentContainer.get())->reorderItems();
	}
	setNiceName(String((int)id->getValue()) + " - " + n);
}

