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
#include "UserInputManager.h"

Cue::Cue(var params) :
	BaseItem(params.getProperty("name", "Cue 1")),
	objectType(params.getProperty("type", "Cue").toString()),
	objectData(params),
	actionsContainer("Actions"),
	commands("Commands"),
	tasks("Tasks"),
	timingContainer("Timing")
{
	canBeDisabled = false;
	saveAndLoadRecursiveData = true;
	editorIsCollapsed = false;
	itemDataType = "Cue";
	
	id = addFloatParameter("ID", "Id of this cue", 0, 0);

	goText = addStringParameter("Go text", "Fill here the action on stage needed for the cue", "");
	cueText = addStringParameter("Cue text", "What's happening during this cue ?", "");
	cueText->multiline = true;
	lastTriggeredTS = addStringParameter("Last triggered", "When did this cue was triggered for the last time ?", "");
	lastTriggeredTS->enabled = false;

	autoFollow = addEnumParameter("Auto Follow", "Does the cuelist stops the execution of the cuelist or auto triggers the next one");
	autoFollow->addOption("Wait for go", "none");
	autoFollow->addOption("End of transitions", "auto");
	autoFollow->addOption("Immediate", "immediate");
	autoFollowTiming = addFloatParameter("Auto Follow delay", "Number of seconds before trigger the auto go ", 0, 0);
	autoFollowCountDown = addFloatParameter("Auto Follow CountdDown", "Triggers next cue when arrives to 0", 0, 0);
	autoFollowCountDown->isControllableFeedbackOnly = true;

	canBeRandomlyCalled = addBoolParameter("Random callable", "Can this cue be called by the randomGo of its cuelist ?", true);
	loadWindowBreakLine = addBoolParameter("New line load window", "If checked, this element will force a new line in the cuelist window", false);
	goBtn = actionsContainer.addTrigger("GO", "trigger this cue");
	cleanUnusedCommandsBtn = actionsContainer.addTrigger("Clean", "Clean the duplicates commands in this cue.");
	loadBtn = actionsContainer.addTrigger("Load content", "load the content of this cue in programmer");
	replaceBtn = actionsContainer.addTrigger("Replace", "The content of this cue is deleted and replaced with actual content of programmer");
	mergeBtn = actionsContainer.addTrigger("Merge", "The content of the programmer is added to this cue");
	createBeforeBtn = actionsContainer.addTrigger("Create Before", "Create a cue before this one with the content of the programmer");
	createAfterBtn = actionsContainer.addTrigger("Create After", "Create a cue after this one with the content of the programmer");

	commands.selectItemWhenCreated = false;
	tasks.selectItemWhenCreated = false;

	htpInDelay = timingContainer.addFloatParameter("HTP in delay", "Default delay for HTP rising values",0,0);
	htpOutDelay = timingContainer.addFloatParameter("HTP out delay", "Default delay for HTP falling values (-1 means use HTP in delay)", -1, -1);
	ltpDelay = timingContainer.addFloatParameter("LTP delay", "Default delay for LTP values (-1 means use HTP in delay)", -1, -1);
	htpInFade = timingContainer.addFloatParameter("HTP in fade", "Default delay for HTP rising values", 0, 0);
	htpOutFade = timingContainer.addFloatParameter("HTP out fade", "Default delay for HTP falling values (-1 means use HTP in fade)", -1, -1);
	ltpFade = timingContainer.addFloatParameter("LTP fade", "Default delay for LTP values (-1 means use HTP in fade)", -1, -1);

	timingContainer.editorIsCollapsed = true;

	addChildControllableContainer(&actionsContainer);
	addChildControllableContainer(&timingContainer);

	addChildControllableContainer(&commands);
	addChildControllableContainer(&tasks);

	if (params.isVoid()) {
		commands.addItem();
	}
}

Cue::~Cue()
{
//	LOG("delete cue");
	Brain::getInstance()->usingCollections.enter();
	Brain::getInstance()->cuePoolWaiting.removeAllInstancesOf(this);
	Brain::getInstance()->cuePoolUpdating.removeAllInstancesOf(this);
	Brain::getInstance()->usingCollections.exit();

}


void Cue::triggerTriggered(Trigger* t) {
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
			MessageManager::callAsync([this, parentCuelist]() {parentCuelist->reorderCues(); });
		}
	}
}

void Cue::onControllableFeedbackUpdate(ControllableContainer* cc, Controllable* c)
{
	if (c == goBtn) {
		Cuelist* parentCuelist = dynamic_cast<Cuelist*>(this->parentContainer->parentContainer.get());
		Cue* temp = this;
		parentCuelist->go(temp);
	}
	else if (c == cleanUnusedCommandsBtn) {
		computeValues();
		cleanUnused();
	}
	else if (c == loadBtn) {
		Programmer* p = UserInputManager::getInstance()->getProgrammer(false);
		loadContent(p);
	}
	else if (c == replaceBtn) {
		const MessageManagerLock mmlock;
		Programmer* p = UserInputManager::getInstance()->getProgrammer(false);
		if (p != nullptr) {
			commands.clear();
			for (int i = 0; i < p->commands.items.size(); i++) {
				Command* com = commands.addItem();
				com->loadJSONData(p->commands.items[i]->getJSONData());
			}
			Cuelist* parentCuelist = dynamic_cast<Cuelist*>(this->parentContainer->parentContainer.get());
			if (parentCuelist->cueA == this) {
				parentCuelist->go(this,0,0);
			}
		}
	}
	else if (c == mergeBtn) {
		const MessageManagerLock mmlock;
		Programmer* p = UserInputManager::getInstance()->getProgrammer(false);
		if (p != nullptr) {
			for (int i = 0; i < p->commands.items.size(); i++) {
				Command* com = commands.addItem();
				com->loadJSONData(p->commands.items[i]->getJSONData());
			}
			Cuelist* parentCuelist = dynamic_cast<Cuelist*>(this->parentContainer->parentContainer.get());
			if (parentCuelist->cueA == this) {
				parentCuelist->go(this, 0, 0);
			}
		}
	}
	else if (c == createBeforeBtn) {
		Cuelist* parentCuelist = dynamic_cast<Cuelist*>(this->parentContainer->parentContainer.get());
		parentCuelist->insertProgCueBefore(this);
	}
	else if (c == createAfterBtn) {
		Cuelist* parentCuelist = dynamic_cast<Cuelist*>(this->parentContainer->parentContainer.get());
		parentCuelist->insertProgCueAfter(this);
	}
}

void Cue::computeValues() {
	csComputing.enter();
	isComputing = true;
	maxTiming = 0;
	computedValues.clear();
	channelToCommand.clear();
	Cuelist* parentCuelist = dynamic_cast<Cuelist*>(this->parentContainer->parentContainer.get());
	Array<Command*> cs = commands.getItemsWithType<Command>();
	for (int i = 0; i < cs.size(); i++) {
		cs[i]->computeValues(parentCuelist, this);
		maxTiming = std::max(maxTiming, cs[i]->maxTiming);
		for (auto it = cs[i]->computedValues.begin(); it != cs[i]->computedValues.end(); it.next()) {
			SubFixtureChannel* fc = it.getKey();
			if (fc != nullptr) {
				computedValues.set(fc, it.getValue());
				channelToCommand.set(fc, cs[i]);
			}
		}
	}
	csComputing.exit();
	isComputing = false;
}

void Cue::go() {
	double now = Brain::getInstance()->now;
	if (autoFollow->getValue() == "immediate") {
		TSAutoFollowStart = now;
		float delay = autoFollowTiming->getValue();
		TSAutoFollowEnd = now + (delay*1000);
		Brain::getInstance()->pleaseUpdate(this);
	}
	Cuelist* parentCuelist = dynamic_cast<Cuelist*>(this->parentContainer->parentContainer.get());
	for (int i = 0; i < tasks.items.size(); i++) {
		if (tasks.items[i]->enabled->boolValue()) {
			Brain::getInstance()->startTask(tasks.items[i], now, parentCuelist->id->intValue());
		}
	}
}

void Cue::update(double now) {
	if (TSAutoFollowEnd != 0 && now < TSAutoFollowEnd) {
		if (TSAutoFollowEnd - TSAutoFollowStart > 10) {
			float remaining = TSAutoFollowEnd - now;
			autoFollowCountDown->setValue(remaining);
		}
		Brain::getInstance()->pleaseUpdate(this);
	}
	else {
		double delta = now - TSAutoFollowEnd;
		TSAutoFollowEnd = 0;
		autoFollowCountDown->setValue(0);
		Cuelist* parentCuelist = dynamic_cast<Cuelist*>(this->parentContainer->parentContainer.get());
		if (!parentCuelist->wannaOff && parentCuelist->cueA == this) {
			parentCuelist->TSLateCompensation;
			parentCuelist->go();
		}
	}
}

void Cue::endTransition() {
	try {
		if (objectType == "Cue" && autoFollow != nullptr && autoFollow->getValue() == "auto") { // bug ici au load
			double now = Brain::getInstance()->now;
			TSAutoFollowStart = now;
			float delay = autoFollowTiming->getValue();
			TSAutoFollowEnd = now + (delay*1000);
			Brain::getInstance()->pleaseUpdate(this);
		}
	}
	catch (...) {
	}
}

void Cue::cleanUnused()
{
	Array<Command*> usedCommands;
	csComputing.enter();

	for (int i = 0; i < commands.items.size(); i++) {
		commands.items[i]->cleanUnused();
	}

	for (auto it = channelToCommand.begin(); it != channelToCommand.end(); it.next()) {
		usedCommands.addIfNotAlreadyThere(it.getValue());
	}

	for (int i = commands.items.size() - 1; i >= 0; i--) {
		Command* c = commands.items[i];
		if (!usedCommands.contains(c)) {
			commands.removeItem(c);
		}
	}

	csComputing.exit();
}

void Cue::loadContent(Programmer* p)
{
	MessageManager::callAsync([this, p](){
		if (p != nullptr) 
			{
			p->clearAll();
			for (int i = 0; i < commands.items.size(); i++) 
				{
				Command* com = p->commands.addItem();
				com->loadJSONData(commands.items[i]->getJSONData());
				}

			p->selectNextCommand();
			UserInputManager::getInstance()->programmerCommandStructureChanged(p);
			}
		});
}

void Cue::writeTimeStamp()
{
	MessageManager::callAsync([this](){
		lastTriggeredTS->setValue(Time::getCurrentTime().formatted("%Y-%m-%d %H:%M:%S"));
	});
}
