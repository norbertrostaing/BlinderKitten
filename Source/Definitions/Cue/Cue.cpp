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
#include "BKEngine.h"
#include "UI/CuelistSheet/CuelistSheet.h"
#include "Cuelist/CuelistManager.h"
#include "ChannelValue.h"

Cue::Cue(var params) :
	BaseItem(params.getProperty("name", "Cue 1")),
	objectType(params.getProperty("type", "Cue").toString()),
	objectData(params),
	actionsContainer("Actions"),
	commands("Commands"),
	tasks("Tasks"),
	tasksOffCue("Tasks off"),
	timingContainer("Timing")
{
	canBeDisabled = false;
	saveAndLoadRecursiveData = true;
	editorIsCollapsed = false;
	itemDataType = "Cue";
	
	id = addFloatParameter("ID", "Id of this cue", 1, 0);

	goText = addStringParameter("Go text", "Fill here the action on stage needed for the cue", "");
	cueText = addStringParameter("Cue text", "What's happening during this cue ?", "");
	cueText->multiline = true;
	lastTriggeredTS = addStringParameter("Last triggered", "When did this cue was triggered for the last time ?", "");
	lastTriggeredTS->enabled = false;

	releaseCurrentTracking = addBoolParameter("Release Tracking", "If checked, all tracked values will be released when this cue is triggered", false);
	
	reuseCue = addTargetParameter("Use cue commands", "The commands of selected cue will be appplied before this cue", CuelistManager::getInstance());
	reuseCue->targetType = TargetParameter::CONTAINER;
	reuseCue->maxDefaultSearchLevel = 2;
	reuseCue->typesFilter = StringArray("Cue");

	autoFollow = addEnumParameter("Auto Follow", "Does the cuelist stops the execution of the cuelist or auto triggers the next one");
	autoFollow->addOption("Wait for go", "none");
	autoFollow->addOption("End of transitions", "auto");
	autoFollow->addOption("Immediate", "immediate");
	autoFollowTiming = addFloatParameter("Auto Follow delay", "Number of seconds before trigger the auto go ", 0, 0);
	autoFollowCountDown = addFloatParameter("Auto Follow Countdown", "Triggers next cue when arrives to 0", 0, 0);
	autoFollowCountDown->isControllableFeedbackOnly = true;

	canBeRandomlyCalled = addBoolParameter("Random callable", "Can this cue be called by the randomGo of its cuelist ?", true);
	loadWindowBreakLine = addBoolParameter("New line load window", "If checked, this element will force a new line in the cuelist window", false);
	goBtn = actionsContainer.addTrigger("GO", "trigger this cue");
	cleanUnusedCommandsBtn = actionsContainer.addTrigger("Clean", "Clean the duplicates commands in this cue.");
	regroupCommandsBtn = actionsContainer.addTrigger("Regroup", "Regroup similar commands on fixtures to corresponding group.");
	takeSelectionBtn = actionsContainer.addTrigger("Take Selection", "Get this cue's selections in the programmer.");
	loadBtn = actionsContainer.addTrigger("Load content", "load the content of this cue in programmer");
	replaceBtn = actionsContainer.addTrigger("Replace", "The content of this cue is deleted and replaced with actual content of programmer");
	mergeBtn = actionsContainer.addTrigger("Merge", "The content of the programmer is added to this cue");
	createBeforeBtn = actionsContainer.addTrigger("Create Before", "Create a cue before this one with the content of the programmer");
	createAfterBtn = actionsContainer.addTrigger("Create After", "Create a cue after this one with the content of the programmer");

	commands.selectItemWhenCreated = false;
	tasks.selectItemWhenCreated = false;
	tasksOffCue.selectItemWhenCreated = false;

	htpInDelay = timingContainer.addFloatParameter("HTP in delay", "Default delay for HTP rising values", -1, -1);
	htpOutDelay = timingContainer.addFloatParameter("HTP out delay", "Default delay for HTP falling values (-1 means use HTP in delay)", -1, -1);
	ltpDelay = timingContainer.addFloatParameter("LTP delay", "Default delay for LTP values (-1 means use HTP in delay)", -1, -1);
	htpInFade = timingContainer.addFloatParameter("HTP in fade", "Default delay for HTP rising values",  -1,  -1);
	htpOutFade = timingContainer.addFloatParameter("HTP out fade", "Default delay for HTP falling values (-1 means use HTP in fade)", -1, -1);
	ltpFade = timingContainer.addFloatParameter("LTP fade", "Default delay for LTP values (-1 means use HTP in fade)", -1, -1);

	moveInBlackDelay = addFloatParameter("MIB Delay", "Delay to wait after light goes off to trigger move in black", -1, -1);

	addChildControllableContainer(&actionsContainer);
	addChildControllableContainer(&timingContainer);
	addChildControllableContainer(&moveInBlack);

	addChildControllableContainer(&commands);
	addChildControllableContainer(&tasks);
	addChildControllableContainer(&tasksOffCue);

	if (params.isVoid()) {
		commands.addItem();
	}
	dynamic_cast<BKEngine*>(BKEngine::mainEngine)->selectCue(this, BKEngine::SET);
}

Cue::~Cue()
{
//	LOG("delete cue");
	Brain::getInstance()->usingCollections.enter();
	Brain::getInstance()->cuePoolWaiting.removeAllInstancesOf(this);
	Brain::getInstance()->cuePoolUpdating.removeAllInstancesOf(this);
	Brain::getInstance()->usingCollections.exit();
	dynamic_cast<BKEngine*>(BKEngine::mainEngine)->selectedCues.removeAllInstancesOf(this);
	if (CuelistSheet::getInstanceWithoutCreating()) {
		CuelistSheet::getInstance()->cueDeleted(this);
	}
	if (this->parentContainer != nullptr && this->parentContainer->parentContainer != nullptr) {
		checkParentCuelist();
		if (parentCuelist->cueA == this) {
			parentCuelist->kill();
		}
	}

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
			checkParentCuelist();
			parentCuelist->forceCueId(this, id->floatValue());
			MessageManager::callAsync([this]() {
				parentCuelist->reorderCues(); 
			});
			sendChangeMessage();
		}
	}
	else if (p == releaseCurrentTracking && parentCuelist!= nullptr && parentCuelist->cueA != nullptr) {
		parentCuelist->go(parentCuelist->cueA, 0,0);
	}
}

void Cue::onControllableFeedbackUpdate(ControllableContainer* cc, Controllable* c)
{
	if (c == goBtn) {
		checkParentCuelist();
		Cue* temp = this;
		parentCuelist->go(temp);
	}
	else if (c == cleanUnusedCommandsBtn) {
		computeValues();
		cleanUnused();
	}
	else if (c == regroupCommandsBtn) {
		regroupCommands();
	}
	else if (c == takeSelectionBtn) {
		takeSelection(nullptr);
	}
	else if (c == loadBtn) {
		Programmer* p = UserInputManager::getInstance()->getProgrammer(false);
		loadContent(p);
	}
	else if (c == replaceBtn) {
		const MessageManagerLock mmlock;
		Programmer* p = UserInputManager::getInstance()->getProgrammer(false);
		if (p != nullptr) {
			replaceContent(p);
		}
	}
	else if (c == mergeBtn) {
		const MessageManagerLock mmlock;
		Programmer* p = UserInputManager::getInstance()->getProgrammer(false);
		if (p != nullptr) {
			mergeContent(p);
		}
	}
	else if (c == createBeforeBtn) {
		checkParentCuelist();
		parentCuelist->insertProgCueBefore(this);
	}
	else if (c == createAfterBtn) {
		checkParentCuelist();
		parentCuelist->insertProgCueAfter(this);
	}
	else if (c == htpInDelay || c == htpOutDelay || c == ltpDelay || c == htpInFade || c == htpOutFade || c == ltpFade) {
		sendChangeMessage();
	}
}

void Cue::computeValues() {
	Array<Cue*> histo;
	computeValues(histo, this);
}

void Cue::computeValues(Array<Cue*> history, Cue* callingCue) {
	if (history.contains(this)) return;
	history.add(this);
	csComputing.enter();
	isComputing = true;
	maxTiming = 0;
	computedValues.clear();
	channelToCommand.clear();
	commandHistory.clear();
	if (callingCue == nullptr) callingCue = this;

	if (reuseCue->getValue() != "") {
		Cue* original = dynamic_cast<Cue*>(reuseCue->targetContainer.get());
		if (original != nullptr) {
			original->computeValues(history, callingCue);
			original->csComputing.enter();
			for (auto it = original->computedValues.begin(); it != original->computedValues.end(); it.next()) {
				computedValues.set(it.getKey(), it.getValue());
			}
			for (auto it = original->channelToCommand.begin(); it != original->channelToCommand.end(); it.next()) {
				channelToCommand.set(it.getKey(), it.getValue());
			}
			commandHistory.addArray(original->commandHistory);
			original->csComputing.exit();
		}
	}

	checkParentCuelist();
	Array<Command*> cs = commands.getItemsWithType<Command>();
	for (int i = 0; i < cs.size(); i++) {
		commandHistory.removeAllInstancesOf(cs[i]);
		commandHistory.add(cs[i]);
		cs[i]->computeValues(parentCuelist, callingCue);
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

void Cue::go() 
{
	go (-1, -1); 
}

void Cue::go(float forcedDelay, float forcedFade)
{
	checkParentCuelist();
	double now = Brain::getInstance()->now;
	if (autoFollow->getValue() == "immediate") {
		TSAutoFollowStart = now;
		float delay = autoFollowTiming->getValue();
		if (delay > 0) {
			float mult = parentCuelist->speedMult.getValue();
			delay *= mult;
		}
		if (delay == 0) {delay = 0.05;}
		TSAutoFollowEnd = now + (delay * 1000);
		Brain::getInstance()->pleaseUpdate(this);
	}
	runTasks(forcedDelay, forcedFade);
}

void Cue::off()
{
	off(-1, -1);
}

void Cue::off(float forcedDelay, float forcedFade)
{
	runOffTasks(forcedDelay, forcedFade);
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
		TSAutoFollowEnd = 0;
		autoFollowCountDown->setValue(0);
		checkParentCuelist();
		if (!parentCuelist->wannaOff && parentCuelist->cueA == this) {
			parentCuelist->TSLateCompensation;
			parentCuelist->go();
		}
	}

	if (parentCuelist->isChaser->boolValue() && parentCuelist->isCuelistOn->boolValue()) {
		if (parentCuelist->lastGoTS + parentCuelist->chaserStepDuration + 500 < now) {
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
			checkParentCuelist();
			float mult = parentCuelist->speedMult.getValue();
			delay *= mult;
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
	computeValues();
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

void Cue::regroupCommands()
{
	computeValues();

	for (auto it = Brain::getInstance()->groups.begin(); it != Brain::getInstance()->groups.end(); it.next()) {
		it.getValue()->selection.computeSelection();
	}
	
	Array<int> dontProcess;
	Array<Command*> toDelete;

	for (int i = 0; i < commands.items.size(); i++) {
		if (dontProcess.contains(i)) continue;
		Command* cmdOrigin = commands.items[i];
		if (cmdOrigin->selection.items.size() != 1) continue;
		if (cmdOrigin->selection.items[0]->targetType->getValueData() != "fixture") continue;
		if (cmdOrigin->values.items.size() != 1) continue;
		if (cmdOrigin->values.items[0]->thru->boolValue()) continue;

		for (int j = i + 1; j < commands.items.size(); j++) {
			Command* cmdTest = commands.items[j];
			if (cmdTest->selection.items.size() != 1) continue;
			if (cmdTest->selection.items[0]->targetType->getValueData() != "fixture") continue;
			if (cmdTest->values.items.size() != 1) continue;
			if (cmdTest->values.items[0]->thru->boolValue()) continue;

			if (cmdOrigin->values.items[0]->presetOrValue->getValue() != cmdTest->values.items[0]->presetOrValue->getValue()) continue;
			if (cmdOrigin->values.items[0]->presetOrValue->getValue() == "preset") {
				if (cmdOrigin->values.items[0]->presetIdFrom->getValue() != cmdTest->values.items[0]->presetIdFrom->getValue()) continue;
			}
			else if (cmdOrigin->values.items[0]->presetOrValue->getValue() == "value") {
				if (cmdOrigin->values.items[0]->valueFrom->getValue() != cmdTest->values.items[0]->valueFrom->getValue()) continue;
			}

			cmdOrigin->selection.addItemFromData(cmdTest->selection.items[0]->getJSONData());
			dontProcess.add(j);
			toDelete.add(cmdTest);
		}

	}
	commands.removeItems(toDelete);

	toDelete.clear();
	Array<var> toAdd;

	for (Command* cmd : commands.items) {
		if (cmd->selection.items.size() > 1) {
			cmd->selection.computeSelection();
			bool replaced = false;

			// check for one corresponding group
			for (auto it = Brain::getInstance()->groups.begin(); it != Brain::getInstance()->groups.end() && !replaced; it.next()) {
				Group* g = it.getValue();
				if (g->selection.computedSelectedSubFixtures.size() != cmd->selection.computedSelectedFixtures.size()) continue;
				bool valid = true;
				for (SubFixture* sf : g->selection.computedSelectedSubFixtures) {
					valid = valid && cmd->selection.computedSelectedSubFixtures.contains(sf);
				}
				if (valid) {
					cmd->selection.clear();
					CommandSelection* cs = cmd->selection.addItem();
					cs->targetType->setValueWithData("group");
					cs->valueFrom->setValue(g->id->intValue());
					replaced = true;
				}
			}

			if (!replaced) {
				Array<Group*> allGroups;
				for (auto it = Brain::getInstance()->groups.begin(); it != Brain::getInstance()->groups.end(); it.next()) {
					allGroups.add(it.getValue());
				}
				Array<Group*> selectedGroups;
				for (int i = 0; i < allGroups.size() && !replaced; i++) {
					selectedGroups.clear();
					Group* firstGroup = allGroups[i];
					Array<SubFixture*> remainingSelection;
					remainingSelection.addArray(cmd->selection.computedSelectedSubFixtures);

					bool valid = true;
					for (SubFixture* sf : firstGroup->selection.computedSelectedSubFixtures) {
						valid = valid && remainingSelection.contains(sf);
						remainingSelection.removeAllInstancesOf(sf);
					}
					if (!valid) continue;
					selectedGroups.add(firstGroup);

					for (int j = i + 1; j < allGroups.size(); j++) {
						Group* secondGroup = allGroups[j];
						for (SubFixture* sf : secondGroup->selection.computedSelectedSubFixtures) {
							valid = valid && remainingSelection.contains(sf);
						}
						if (valid) {
							selectedGroups.add(secondGroup);
							for (SubFixture* sf : secondGroup->selection.computedSelectedSubFixtures) {
								remainingSelection.removeAllInstancesOf(sf);
							}
						}
					}

					if (remainingSelection.size() == 0) {
						replaced = true;
						toDelete.add(cmd);
						cmd->selection.clear();
						CommandSelection* cs = cmd->selection.addItem();
						cs->targetType->setValueWithData("group");

						for (Group* g : selectedGroups) {
							cs->valueFrom->setValue(g->id->intValue());
							toAdd.add(cmd->getJSONData());
						}
					}
				}

			}
		}
	}

	commands.removeItems(toDelete);
	for (var v : toAdd) {
		commands.addItemFromData(v);
	}

}

void Cue::takeSelection(Programmer* p)
{
	if (p == nullptr) p = UserInputManager::getInstance()->getProgrammer(true);
	MessageManager::callAsync([this, p]() {
		Command* cmd = p->currentUserCommand;
		if (cmd == nullptr || !cmd -> userCanPressSelectionType) {
			cmd = p->commands.addItem();
			cmd->selection.clear();
		}
		for (Command* c : commands.items) {
			for (CommandSelection* cs : c->selection.items) {
				CommandSelection* newCs  = cmd->selection.addItem();
				newCs->loadJSONData(cs->getJSONData());
			}
		}
		p->checkCurrentUserCommand();
		p->selectCommand(cmd);
		UserInputManager::getInstance()->programmerCommandStructureChanged(p);
		UserInputManager::getInstance()->commandSelectionChanged(cmd);
		});
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

			p->checkCurrentUserCommand();
			UserInputManager::getInstance()->programmerCommandStructureChanged(p);
			LOG("Cue content loaded in programmer");
			}
		});
}

void Cue::replaceContent(Programmer* p)
{
	commands.clear();
	mergeContent(p);
}

void Cue::mergeContent(Programmer* p)
{
	for (int i = 0; i < p->commands.items.size(); i++) {
		Command* com = commands.addItem();
		com->loadJSONData(p->commands.items[i]->getJSONData());
	}
	checkParentCuelist();
	if (parentCuelist->cueA == this) {
		parentCuelist->go(this, 0, 0);
	}
}

void Cue::writeTimeStamp()
{
	MessageManager::callAsync([this](){
		lastTriggeredTS->setValue(Time::getCurrentTime().formatted("%Y-%m-%d %H:%M:%S"));
	});
}

Array<Task*> Cue::getTasks()
{
	Array<Cue*> histo;
	return getTasks(histo);
}

Array<Task*> Cue::getTasks(Array<Cue*> history)
{
	Array<Task*> ret;
	if (history.contains(this)) { return ret; }
	history.add(this);
	Cue* original = dynamic_cast<Cue*>(reuseCue->targetContainer.get());
	if (original != nullptr) {
		ret = original->getTasks(history);
	}
	for (Task* t : tasks.items) {
		ret.add(t);
	}
	return ret;
}

Array<Task*> Cue::getOffTasks()
{
	Array<Cue*> histo;
	return getOffTasks(histo);
}

Array<Task*> Cue::getOffTasks(Array<Cue*> history)
{
	Array<Task*> ret;
	if (history.contains(this)) { return ret; }
	history.add(this);
	Cue* original = dynamic_cast<Cue*>(reuseCue->targetContainer.get());
	if (original != nullptr) {
		ret = original->getOffTasks(history);
	}
	for (Task* t : tasksOffCue.items) {
		ret.add(t);
	}
	return ret;
}

void Cue::runTasks(float forcedDelay, float forcedFade)
{
	checkParentCuelist();
	double now = Brain::getInstance()->now;
	Array<Task*> allTasks = getTasks();
	for (int i = 0; i < allTasks.size(); i++) {
		if (allTasks[i]->enabled->boolValue()) {
			Brain::getInstance()->startTask(allTasks[i], now, parentCuelist->id->intValue(), forcedDelay, forcedFade);
		}
	}
}

void Cue::runOffTasks(float forcedDelay, float forcedFade)
{
	double now = Brain::getInstance()->now;
	checkParentCuelist();
	Array<Task*> allTasks = getOffTasks();
	for (int i = 0; i < allTasks.size(); i++) {
		if (allTasks[i]->enabled->boolValue()) {
			Brain::getInstance()->startTask(allTasks[i], now, parentCuelist->id->intValue(), forcedDelay, forcedFade);
		}
	}
}

String Cue::getCommandsText(bool useName)
{
	Array<Cue*> histo;
	return getCommandsText(useName, histo);
}

String Cue::getCommandsText(bool useName, Array<Cue*> history)
{
	String ret = "";
	if (history.contains(this)) { return ret; }
	history.add(this);
	Cue* original = dynamic_cast<Cue*>(reuseCue->targetContainer.get());
	if (original != nullptr) {
		ret += original->getCommandsText(useName, history);
	}
	for (int i = 0; i < commands.items.size(); i++) {
		if (ret != "") { ret += "\n"; }
		ret += commands.items[i]->getCommandAsTexts(useName).joinIntoString(" ");
	}
	return ret;
}

void Cue::onContainerNiceNameChanged()
{
	sendChangeMessage();
}

void Cue::checkId()
{
	
}

void Cue::checkParentCuelist()
{
	if (parentCuelist == nullptr) {
		parentCuelist = dynamic_cast<Cuelist*>(this->parentContainer->parentContainer.get());
	}
}

