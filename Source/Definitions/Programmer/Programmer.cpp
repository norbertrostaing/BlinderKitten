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
#include "../../UserInputManager.h"
#include "../ChannelValue.h"
#include "../DataTransferManager/DataTransferManager.h"
#include "ProgrammerManager.h"

Programmer::Programmer(var params) :
	BaseItem(params.getProperty("name", "Programmer")),
	objectType(params.getProperty("type", "Programmer").toString()),
	objectData(params),
	commands("Commands"),
	cliContainer("Command LIne")
{
	saveAndLoadRecursiveData = true;
	nameCanBeChangedByUser = false;
	editorIsCollapsed = false;
	canBeDisabled = false;
	itemDataType = "Programmer";

	id = addIntParameter("ID", "Id of this Programmer", 1, 1);
	userName = addStringParameter("Name", "Name of this programmer", "New programmer");
	layerId = addIntParameter("Layer", "Higher layer, higer priority", 1, 1);
	updateName();

	editionMode = addEnumParameter("Edition mode", "mode of edition");
	editionMode->addOption("Not timed", "notTimed");
	editionMode->addOption("Timed", "timed");
	editionMode->addOption("Blind", "blind");

	releaseTime = addFloatParameter("Release time", "Fade time when release values", 0, 0);

	goBtn = addTrigger("GO", "trigger this Programmer");
	releaseBtn = addTrigger("Release", "release this programmer");
	recBtn = addTrigger("Record", "Record the content of this programmer in something");
	clearAllBtn = addTrigger("Clear All", "Clear and reset this programmer");

	cliActionType = cliContainer.addEnumParameter("Action type", "What kind of action do you wanna do ?");
	cliActionType->addOption("None", "");
	cliActionType->addOption("Copy", "copy");
	cliActionType->addOption("Move", "move");
	cliActionType->addOption("Record", "record");
	cliActionType->addOption("Update", "update");
	cliActionType->addOption("Edit", "edit");
	cliActionType->addOption("Delete", "delete");

	cliParamAType = cliContainer.addEnumParameter("Param A type", "What kind of object do you want to target");
	cliParamAType->addOption("None", "");
	cliParamAType->addOption("Group", "group");
	cliParamAType->addOption("Preset", "preset");
	cliParamAType->addOption("Cuelist", "cuelist");
	cliParamAType->addOption("Effect", "effect");
	cliParamAType->addOption("Carousel", "carousel");
	cliParamAType->addOption("Mapper", "mapper");
	cliParamAType->addOption("Virtual button", "virtualbutton");
	cliParamAType->addOption("Virtual fader", "virtualfadercol");
	cliParamAId = cliContainer.addIntParameter("Param A ID", "ID of first param", 0, 0);

	cliParamBType = cliContainer.addEnumParameter("Param B type", "second object type");
	cliParamBType->addOption("None", "");
	cliParamBType->addOption("Group", "group");
	cliParamBType->addOption("Preset", "preset");
	cliParamBType->addOption("Cuelist", "cuelist");
	cliParamBType->addOption("Effect", "effect");
	cliParamBType->addOption("Carousel", "carousel");
	cliParamBType->addOption("Mapper", "mapper");
	cliParamBType->addOption("Virtual button", "virtualbutton");
	cliParamBType->addOption("Virtual fader", "virtualfadercol");
	cliParamBId = cliContainer.addIntParameter("Param B ID", "second object id for copy, move", 0, 0);

	cliGo = cliContainer.addTrigger("GO", "Execute this command");

	commands.selectItemWhenCreated = false;
	addChildControllableContainer(&commands);

	addChildControllableContainer(&cliContainer);

	if (params.isVoid()) {
		commands.addItem();
	}
	isDeleted = false;
	Brain::getInstance()->registerProgrammer(this, id->getValue());
}

Programmer::~Programmer()
{
	isDeleted = true;
	clearAll();
	for (auto it = activeValues.begin(); it != activeValues.end(); it.next()) {
		it.getKey()->programmerOutOfStack(this);
		Brain::getInstance()->pleaseUpdate(it.getKey());
	}
	Brain::getInstance()->unregisterProgrammer(this);
}


void Programmer::triggerTriggered(Trigger* t) {
	if (t == goBtn) {
		go();
	}
	if (t == releaseBtn) {
		release();
	}
	if (t == clearAllBtn) {
		clearAll();
	}
	if (t == recBtn) {
		DataTransferManager::getInstance()->sourceId->setValue(id->getValue());
		DataTransferManager::getInstance()->sourceType->setValue("Programmer");
		DataTransferManager::getInstance()->selectThis();
	}
	if (t == cliGo) {
		runCliCommand();
	}
}

void Programmer::onContainerParameterChangedInternal(Parameter* p) {
	if (p == id) {
		Brain::getInstance()->registerProgrammer(this, id->getValue(), true);
	}
	if (p == userName || p == id) {
		updateName();
	}
	if (p == editionMode) {
		if (editionMode->getValue() == "blind") {
			release();
		}
	}
}

void Programmer::computeValues() {
	maxTiming = 0;
	computedValues.getLock().enter();
	computedValues.clear();
	Array<Command*> cs = commands.getItemsWithType<Command>();
	for (int i = 0; i < cs.size(); i++) {
		cs[i]->computeValues();
		maxTiming = std::max(maxTiming, cs[i]->maxTiming);
		cs[i]->computedValues.getLock().enter();

		for (auto it = cs[i]->computedValues.begin(); it != cs[i]->computedValues.end(); it.next()) {
			SubFixtureChannel* fc = it.getKey();
			if (fc != nullptr) {
				computedValues.set(fc, it.getValue());
			}
		}
		cs[i]->computedValues.getLock().exit();
	}
	computedValues.getLock().exit();
}

void Programmer::go() {
	Brain::getInstance()->pleaseUpdate(this);
}

void Programmer::update(double now) {
	render(now);
}

void Programmer::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) {
	Brain::getInstance()->pleaseUpdate(this);
	if (c->niceName == "Channel type" || c->niceName == "Thru") {
		UserInputManager::getInstance()->programmerCommandStructureChanged(this);
	} 
	else if (c->niceName != "Value"&& c->niceName != "Value to")
	{
		UserInputManager::getInstance()->programmerCommandValueChanged(this);
	}
}

void Programmer::render(double now) {
	ScopedLock lock(computing);
	release(now);
	computeValues();
	String mode = editionMode->getValue();
	if (mode != "blind") {
		for (auto it = computedValues.begin(); it != computedValues.end(); it.next()) {
			ChannelValue* temp = it.getValue();
			if (activeValues.contains(it.getKey())) {
				temp->startValue = it.getKey()->value;
			}
			else {
				temp->startValue = -1;
			}
			temp->TSInit = now;
			if (mode == "timed") {
				temp->TSStart = now + (temp->delay);
				temp->TSEnd = temp->TSStart + (temp->fade);
				temp->isEnded = false;
			}
			else {
				temp->TSStart = now;
				temp->TSEnd = now;
				temp->isEnded = true;
			}
			activeValues.set(it.getKey(), temp);
			it.getKey()->programmerOnTopOfStack(this);
			Brain::getInstance()->pleaseUpdate(it.getKey());
		}

	}

}

void Programmer::release(double now) {
	if (now == 0) {
		now = Brain::getInstance()->now;
	}
	for (auto it = activeValues.begin(); it != activeValues.end(); it.next()) {
		ChannelValue* temp = it.getValue();
		float fadeTime = releaseTime->getValue();

		temp->TSInit = now;
		temp->TSStart = now;
		temp->TSEnd = now + fadeTime;

		temp->endValue = -1;
		temp->startValue = temp->value;
		temp->isEnded = false;

		activeValues.set(it.getKey(), temp);
		Brain::getInstance()->pleaseUpdate(it.getKey());
	}

}


float Programmer::applyToChannel(SubFixtureChannel* fc, float currentVal, double now) {
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
		float totTime = (cv->delay + cv->fade);
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

void Programmer::updateName() {
	String n = userName->getValue();
	if (parentContainer != nullptr) {
		dynamic_cast<ProgrammerManager*>(parentContainer.get())->reorderItems();
	}
	setNiceName(String((int)id->getValue()) + " - " + n);
}


void Programmer::clearAll() {
	release();
	computing.enter();
	currentUserCommand = nullptr;
	commands.clear();
	//commands.items.clear();
	currentUserCommand = nullptr;
	computing.exit();
	Brain::getInstance()->pleaseUpdate(this);
	UserInputManager::getInstance()->commandValueChanged(currentUserCommand);
	UserInputManager::getInstance()->commandSelectionChanged(currentUserCommand);
}

void Programmer::clearCurrent() {
	computing.enter();
	const MessageManagerLock mmLock;

	if (commands.items.size() > 0 && currentUserCommand != nullptr) {
		commands.removeItem(currentUserCommand, false, true);
		commands.items.remove(commands.items.indexOf(currentUserCommand));
	}

	if (commands.items.size() == 0) {
		currentUserCommand = nullptr; //commands.addItem();
	}
	else {
		currentUserCommand = commands.items.getLast();
	}
	computing.exit();
	Brain::getInstance()->pleaseUpdate(this);
	UserInputManager::getInstance()->commandValueChanged(currentUserCommand);
	UserInputManager::getInstance()->commandSelectionChanged(currentUserCommand);
}

void Programmer::selectNextCommand()
{
	checkCurrentUserCommand();
	if (commands.items.size() > 1) {
		int index = commands.items.indexOf(currentUserCommand);
		if (index != -1) {
			index = (index+1)% commands.items.size();
		}
		else {
			index = 0;
		}
		currentUserCommand = commands.items[index];
	}
}

void Programmer::selectPrevCommand()
{
	checkCurrentUserCommand();
	if (commands.items.size() > 1) {
		int index = commands.items.indexOf(currentUserCommand);
		if (index != -1) {
			index = (commands.items.size() + index - 1) % commands.items.size();
		}
		else {
			index = 0;
		}
		currentUserCommand = commands.items[index];
	}
}

void Programmer::processUserInput(String s) {
	s = s.toLowerCase();
	const MessageManagerLock mmLock;
	if (cliActionType->getValue() != "") {
		getCliAsTexts();
		String action = cliActionType->getValue();
		if (s.containsOnly("1234567890")) {
			if (userCanPressNumber) {
				String val = currentUserTargetId->getValue().toString();
				val += s;
				currentUserTargetId->setValue(val.getIntValue());
			}
			else {
				LOGERROR("not allowed");
			}
		}
		else if (s == "fixture" || s == "group" || s == "preset" || s == "cuelist" || s == "effect" || s == "carousel" || s == "mapper" || s == "virtualbutton" || s == "virtualfadercol") {
			if (userCanPressTargetType) {
				dynamic_cast<EnumParameter*>(currentUserTargetType)->setValueWithData(s);
			}
			else {
				LOGERROR("not allowed");
			}
		}
		else if (s == "enter") {
			if (userCanPressGo) {
				runCliCommand();
			}
			else {
				LOGERROR("Invalid command");
			}
		}
		else if (s == "backspace") {
			if (cliLastTarget == "actionType") { cliActionType->setValueWithData(""); }
			else if (cliLastTarget == "paramAType") { cliParamAType->setValueWithData(""); }
			else if (cliLastTarget == "paramBType") { cliParamBType->setValueWithData(""); }
			else if (cliLastTarget == "paramAId") { cliParamAId->setValue(UserInputManager::backspaceOnInt(cliParamAId->getValue())); }
			else if (cliLastTarget == "paramBId") { cliParamBId->setValue(UserInputManager::backspaceOnInt(cliParamBId->getValue())); }
		}
		else if (s == "record") {
			if (userCanPressAction) {
				if (action == "record") { cliActionType->setValueWithData("update"); }
				else { cliActionType->setValueWithData("record"); }
			}
			else {
				LOGERROR("not allowed");
			}
		}
		else if (s == "copy") {
			if (userCanPressAction) {
				if (action == "copy") { cliActionType->setValueWithData("move"); }
				else { cliActionType->setValueWithData("copy"); }
			}
			else {
				LOGERROR("not allowed");
			}
		}
		else if (s == "edit") {
			if (userCanPressAction) {
				cliActionType->setValueWithData(s);
			}
			else {
				LOGERROR("not allowed");
			}
		}
		else if (s == "delete") {
			if (userCanPressAction) {
				cliActionType->setValueWithData(s);
			}
			else {
				LOGERROR("not allowed");
			}
		}

		UserInputManager::getInstance()->updateCommandLine();
		return;
	}

	if (s == "record" || s == "copy" || s == "edit" || s == "delete") {
		cliActionType->setValueWithData(s);
		UserInputManager::getInstance()->updateCommandLine();
	}
	checkCurrentUserCommand();
	currentUserCommand->getCommandAsTexts();
	if (s == "clear") {
		Brain::getInstance()->pleaseClearProgrammer = true;
	}
	else if (s == "+" || s == "-") {
		if (currentUserCommand->userCanPressPlusOrMinus) {
			currentUserCommand->userPress(s);
		}
		else {
			LOGERROR("not allowed");
		}
	}
	else if (s == "group" || s == "fixture") {
		if (currentUserCommand->userCanPressSelectionType) {
			currentUserCommand->userPress(s);
		}
		else if (currentUserCommand->userCanHaveAnotherCommand) {
			currentUserCommand = commands.addItem();
			currentUserCommand->userPress(s);
		}
		else if (currentUserCommand->userCanPressTimingType) {
			currentUserCommand = commands.addItem();
			currentUserCommand->userPress(s);
		}
		else {
			LOGERROR("not allowed");
		}
	}
	else if (s == "backspace") {
		currentUserCommand->userPress(s);
		UserInputManager::getInstance()->commandValueChanged(currentUserCommand);
		UserInputManager::getInstance()->commandSelectionChanged(currentUserCommand);
	}
	else if (s == "subfixture") {
		if (currentUserCommand->userCanPressSubSelection) {
			currentUserCommand->userPress(s);
		}
		else {
			LOGERROR("not allowed");
		}
	}
	else if (s == "thru") {
		if (currentUserCommand->userCanPressThru) {
			currentUserCommand->userPress(s);
		}
		else {
			LOGERROR("not allowed");
		}
	}
	else if (s == "preset") {
		if (currentUserCommand->userCanPressValueType) {
			currentUserCommand->userPress(s);
		}
		else {
			LOGERROR("not allowed");
		}
	}
	else if (s.containsOnly("1234567890.")) {
		if (currentUserCommand->userCanPressValue) {
			currentUserCommand->userPress(s);
		}
		else if (currentUserCommand->userCanPressNumber && s.containsOnly("1234567890")) {
			currentUserCommand->userPress(s);
		}
		else {
			LOGERROR("not allowed");
		}
	}
	else {
		// probably a channel type;
	}
	// currentUserCommand->userPress(s);
}

String Programmer::getTextCommand() {
	StringArray txts;
	String txt = "";
	if (cliActionType->getValue() != "") {
		txts = getCliAsTexts();
	}
	else if (currentUserCommand == nullptr) { return ""; }
	else {
		txts = currentUserCommand->getCommandAsTexts();
	}
	for (int i = 0; i < txts.size(); i++) {
		if (i != 0) {
			txt += " ";
		}
		txt += UserInputManager::getInstance()->toUserText(txts[i]);
	}
	return txt;
}

void Programmer::runCliCommand() {
	getCliAsTexts();
	if (!userCanPressGo) { LOGERROR("Invalid command"); return; }
	String action = cliActionType->getValue();
	String targetType = cliParamAType->getValue();
	int targetId = cliParamAId->getValue();
	if (action == "record" || action == "update") {
		DataTransferManager::getInstance()->sourceType->setValueWithData("programmer");
		DataTransferManager::getInstance()->sourceId->setValue(id->getValue());
		DataTransferManager::getInstance()->targetType->setValueWithData(targetType);
		DataTransferManager::getInstance()->targetUserId->setValue(targetId);
		DataTransferManager::getInstance()->groupCopyMode->setValueWithData(action == "record" ? "merge" : "replace");
		DataTransferManager::getInstance()->presetCopyMode->setValueWithData(action == "record" ? "merge" : "replace");
		DataTransferManager::getInstance()->cuelistCopyMode->setValueWithData(action == "record" ? "add" : "update");
		DataTransferManager::getInstance()->execute();
	}
	else if (action == "copy") {
		DataTransferManager::getInstance()->sourceType->setValueWithData(targetType);
		DataTransferManager::getInstance()->sourceId->setValue(targetId);
		DataTransferManager::getInstance()->targetType->setValueWithData(cliParamBType->getValue());
		DataTransferManager::getInstance()->targetUserId->setValue((int)cliParamBId->getValue());
		DataTransferManager::getInstance()->groupCopyMode->setValueWithData(action == "record" ? "merge" : "replace");
		DataTransferManager::getInstance()->presetCopyMode->setValueWithData(action == "record" ? "merge" : "replace");
		DataTransferManager::getInstance()->cuelistCopyMode->setValueWithData(action == "record" ? "add" : "update");
		DataTransferManager::getInstance()->execute();


	}
	else if (action == "move") {
		DataTransferManager::getInstance()->moveObject(targetType, targetId, (int)cliParamBId->getValue());
	}
	else if (action == "edit") {
		DataTransferManager::getInstance()->editObject(targetType, targetId);
	}
	else if (action == "delete") {
		DataTransferManager::getInstance()->deleteObject(targetType, targetId);
	}
	resetCli();
}

StringArray Programmer::getCliAsTexts() {
	StringArray words;
	userCantPress();
	userCanPressAction = true;
	currentUserTargetId = nullptr;
	currentUserTargetType = nullptr;
	cliLastTarget = "";
	String action = cliActionType->getValue();
	if (action != "") {
		words.add(cliActionType->getValue());
		cliLastTarget = "actionType";
		userCanPressTargetType = true;
		currentUserTargetType = cliParamAType;

		if (cliParamAType->getValue() != "") {
			words.add(cliParamAType->getValueData());
			cliLastTarget = "paramAType";
			userCanPressAction = false;
			userCanPressNumber = true;
			currentUserTargetId = cliParamAId;

			if ((int)cliParamAId->getValue() > 0) {
				words.add(cliParamAId->getValue());
				cliLastTarget = "paramAId";
				// value entered
				if (action == "copy" || action == "move") {
					userCanPressTargetType = true;
					currentUserTargetType = cliParamBType;

					if (cliParamBType->getValue() != "") {
						words.add(cliParamBType->getValueData());
						cliLastTarget = "paramBType";
						userCanPressTargetType = false;
						currentUserTargetId = cliParamBId;
						if ((int)cliParamAId->getValue() > 0) {
							cliLastTarget = "paramBId";
							words.add(cliParamBId->getValue());
							userCanPressGo = true;
						}
					}
				}
				else {
					userCanPressGo = true;
				}
			}
		}
	}

	return words;

}

void Programmer::userCantPress() {
	userCanPressAction = false;
	userCanPressTargetType = false;
	userCanPressNumber = false;
	userCanPressGo = false;
}

void Programmer::resetCli() {
	cliActionType->setValueWithData("");
	cliParamAType->setValueWithData("");
	cliParamBType->setValueWithData("");
	cliParamAId->setValue(0);
	cliParamBId->setValue(0);
}

void Programmer::checkCurrentUserCommand() {
	const MessageManagerLock mmLock;
	if (currentUserCommand != nullptr && currentUserCommand->toDelete) {
		currentUserCommand = nullptr;
	}
	if (currentUserCommand == nullptr) {
		if (commands.items.size() > 0) {
			currentUserCommand = commands.items[0];
		}
		else {
			currentUserCommand = commands.addItem();
		}
	}
}