/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "Cue.h"
#include "../Cuelist/Cuelist.h"
#include "../../Brain.h"

Cue::Cue(var params) :
	BaseItem(params.getProperty("name", "Cue")),
	objectType(params.getProperty("type", "Cue").toString()),
	objectData(params),
	commands("Commands"),
	timingContainer("Timing")
{
	saveAndLoadRecursiveData = true;
	editorIsCollapsed = false;
	itemDataType = "Cue";
	
	id = addFloatParameter("ID", "Id of this cue", 0, 0);
	
	autoFollow = addEnumParameter("Auto Follow", "Does the cuelist stops the execution of the cuelist or auto triggers the next one");
	autoFollow->addOption("Wait for go", "none");
	autoFollow->addOption("End of transitions", "auto");
	autoFollow->addOption("Immediate", "immediate");
	autoFollowTiming = addFloatParameter("Auto Follow timing", "Number of seconds before trigger the auto go ", 0, 0);
	autoFollowCountDown = addFloatParameter("Auto Follow CountdDown", "Triggers next cue when arrives to 0", 0, 0);
	autoFollowCountDown->isControllableFeedbackOnly = true;

	canBeRandomlyCalled = addBoolParameter("Random callable", "Can this cue be called by the randomGo of its cuelist ?", true);
	goBtn = addTrigger("GO", "trigger this cue");

	commands.selectItemWhenCreated = false;

	htpInDelay = timingContainer.addFloatParameter("HTP in delay", "Default delay for HTP rising values",0,0);
	htpOutDelay = timingContainer.addFloatParameter("HTP out delay", "Default delay for HTP falling values", 0, 0);
	ltpDelay = timingContainer.addFloatParameter("LTP delay", "Default delay for LTP values", 0, 0);
	htpInFade = timingContainer.addFloatParameter("HTP in fade", "Default delay for HTP rising values", 0, 0);
	htpOutFade = timingContainer.addFloatParameter("HTP out fade", "Default delay for HTP falling values", 0, 0);
	ltpFade = timingContainer.addFloatParameter("LTP fade", "Default delay for LTP values", 0, 0);

	timingContainer.editorIsCollapsed = true;

	addChildControllableContainer(&timingContainer);

	addChildControllableContainer(&commands);

	if (params.isVoid()) {
		commands.addItem();
	}
}

Cue::~Cue()
{
	LOG("delete cue");
}


void Cue::triggerTriggered(Trigger* t) {
	if (t == goBtn) {
		Cuelist* parentCuelist = dynamic_cast<Cuelist*>(this->parentContainer->parentContainer.get());
		Cue* temp = this;
		parentCuelist->go(temp);
	}
}

void Cue::onContainerParameterChangedInternal(Parameter* p) {
	if (p == autoFollowTiming) {
		float v = p->getValue();
		v = jmax((float)1, v);
		autoFollowCountDown->setRange(0,v);
	}
	else if (p == id) {
		if (this->parentContainer != nullptr && this->parentContainer->parentContainer != nullptr) {
			Cuelist* parentCuelist = dynamic_cast<Cuelist*>(this->parentContainer->parentContainer.get());
			parentCuelist->reorderCues();
		}
	}
}

void Cue::computeValues() {
	maxTiming = 0;
	computedValues.clear();
	Cuelist* parentCuelist = dynamic_cast<Cuelist*>(this->parentContainer->parentContainer.get());
	Array<Command*> cs = commands.getItemsWithType<Command>();
	for (int i = 0; i < cs.size(); i++) {
		cs[i]->computeValues(parentCuelist, this);
		maxTiming = std::max(maxTiming, cs[i]->maxTiming);
		for (auto it = cs[i]->computedValues.begin(); it != cs[i]->computedValues.end(); it.next()) {
			SubFixtureChannel* fc = it.getKey();
			computedValues.set(fc, it.getValue());
		}
	}
}

void Cue::go() {
	if (autoFollow->getValue() == "immediate") {
		double now = Brain::getInstance()->now;

		TSAutoFollowStart = now;
		float delay = autoFollowTiming->getValue();
		TSAutoFollowEnd = now + (delay*1000);
		Brain::getInstance()->pleaseUpdate(this);
	}
}

void Cue::update(double now) {
	if (TSAutoFollowEnd != 0 && now < TSAutoFollowEnd) {
		if (TSAutoFollowEnd - TSAutoFollowStart > 200) {
			float remaining = TSAutoFollowEnd - now;
			autoFollowCountDown->setValue(remaining);
		}
		Brain::getInstance()->pleaseUpdate(this);
	}
	else {
		TSAutoFollowEnd = 0;
		autoFollowCountDown->setValue(0);
		Cuelist* parentCuelist = dynamic_cast<Cuelist*>(this->parentContainer->parentContainer.get());
		if (!parentCuelist->wannaOff) {
			parentCuelist->go();
		}
	}
}

void Cue::endTransition() {
	if (autoFollow != nullptr && autoFollow->getValue() == "auto") { // bug ici au load
		double now = Brain::getInstance()->now;
		TSAutoFollowStart = now;
		float delay = autoFollowTiming->getValue();
		TSAutoFollowEnd = now + delay;
		Brain::getInstance()->pleaseUpdate(this);
	}
}