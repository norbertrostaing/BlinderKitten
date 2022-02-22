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
	itemDataType = "Programmer";
	
	id = addIntParameter("ID", "Id of this Programmer", 1, 1);
	userName = addStringParameter("Name", "Name of this programmer", "New programmer");
	updateName();

	editionMode = addEnumParameter("Edition mode", "mode of edition");
	editionMode->addOption("Not timed", "notTimed");
	editionMode->addOption("Timed", "timed");
	editionMode->addOption("Blind", "blind");

	releaseTime = addFloatParameter("Release time", "Fade time when release values",0,0);

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
	cliParamAId = cliContainer.addIntParameter("Param A ID", "ID of first param",0,0);

	cliParamBType = cliContainer.addEnumParameter("Param B type", "second object type");
	cliParamBType->addOption("", "");
	cliParamBType->addOption("Group", "group");
	cliParamBType->addOption("Preset", "preset");
	cliParamBType->addOption("Cuelist", "cuelist");
	cliParamBType->addOption("Effect", "effect");
	cliParamBType->addOption("Carousel", "carousel");
	cliParamBId = cliContainer.addIntParameter("Param B ID", "second object id for copy, move", 0, 0);

	cliGo = cliContainer.addTrigger("GO", "Execute this command");

	commands.selectItemWhenCreated = false;
	addChildControllableContainer(&commands);

	addChildControllableContainer(&cliContainer);

	if (params.isVoid()) {
		commands.addItem();
	}
	Brain::getInstance()->registerProgrammer(this, id->getValue());
}

Programmer::~Programmer()
{
	clearAll();
	for (auto it = activeValues.begin(); it != activeValues.end(); it.next()) {
		it.getKey()->programmerOutOfStack(this);
		Brain::getInstance()->pleaseUpdate(it.getKey());
	}
	Brain::getInstance()->unregisterProgrammer(this);
	LOG("delete Programmer");
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
		DataTransferManager::getInstance()->sourceId ->setValue(id->getValue());
		DataTransferManager::getInstance()->sourceType->setValue("Programmer");
		DataTransferManager::getInstance()->selectThis();
	}
}

void Programmer::onContainerParameterChangedInternal(Parameter* p) {
	if (p == id) {
		Brain::getInstance()->registerProgrammer(this, id->getValue());
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
	computedValues.clear();
	Array<Command*> cs = commands.getItemsWithType<Command>();
	for (int i = 0; i < cs.size(); i++) {
		cs[i]->computeValues();
		maxTiming = std::max(maxTiming, cs[i]->maxTiming);
		for (auto it = cs[i]->computedValues.begin(); it != cs[i]->computedValues.end(); it.next()) {
			SubFixtureChannel* fc = it.getKey();
			computedValues.set(fc, it.getValue());
		}
	}
}

void Programmer::go() {
	Brain::getInstance()->pleaseUpdate(this);
}

void Programmer::update(double now) {
	render(now);
}

void Programmer::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) {
	Brain::getInstance()->pleaseUpdate(this);
}

void Programmer::render(double now) {
	release(now);
	computeValues();
	String mode = editionMode->getValue();
	if (mode != "blind") {
		for (auto it = computedValues.begin(); it != computedValues.end(); it.next()) {
			ChannelValue* temp = it.getValue();
			if (activeValues.contains(it.getKey())) {
				// ChannelValue* current = activeValues.getReference(it.getKey());
				temp->startValue = it.getKey()->value;
			}
			else {
				temp->startValue = -1;
			}
			temp->TSInit = now;
			if (mode == "timed") {
				temp->TSStart = now + (temp->delay );
				temp->TSEnd = temp->TSStart + (temp->fade );
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

void Programmer::release(double now) {
	if (now == 0) {
		now = Brain::getInstance()->now;
	}
	for (auto it = activeValues.begin(); it != activeValues.end(); it.next()) {
		ChannelValue* temp = it.getValue();
		float fadeTime = releaseTime->getValue();

		temp->TSInit = now;
		temp->TSStart = now;
		temp->TSEnd = now +  fadeTime;

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
		float totTime = (cv->delay + cv->fade) ;
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


void Programmer :: clearAll() {
	release();
	commands.clear();
	commands.addItem();
}

void Programmer::processUserInput(String s) {
	s = s.toLowerCase();
	LOG(s);

	if (cliActionType->getValue() != "") {
		getCliAsTexts();
		String action = cliActionType->getValue();
		if (s.containsOnly("1234567890")) {
			if (userCanPressNumber) {
				String val = currentUserTarget -> getValue().toString();
				val += s;
				currentUserTarget->setValue(val.getIntValue());
			}
			else {
				LOGERROR("not allowed");
			}
		}
		else if (s == "fixture" || s == "group" || s == "preset" || s == "cuelist" || s == "effect" || s == "carousel") {
			if (userCanPressTargetType) {
				dynamic_cast<EnumParameter*>(currentUserTarget)->setValueWithData(s);
			}
			else {
				LOGERROR("not allowed");
			}
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
		
	UserInputManager::getInstance() -> updateCommandLine();
	return;
	}

	if (s == "record" || s == "copy" || s == "edit" || s == "delete") {
		cliActionType->setValueWithData(s);
		UserInputManager::getInstance()->updateCommandLine();
	}
	if (currentUserCommand == nullptr) {
		if (commands.items.size() > 0) {
			currentUserCommand = commands.items[0];
		}
		else {
			currentUserCommand = commands.addItem();
		}
	}
	currentUserCommand->getCommandAsTexts();
	if (s == "clear") {
		commands.removeItem(currentUserCommand);
		if (commands.items.size() == 0) {
			commands.addItem();
		}
		currentUserCommand = commands.items.getLast();
		Brain::getInstance()->pleaseUpdate(this);
		UserInputManager::getInstance()->commandValueChanged(currentUserCommand);
		UserInputManager::getInstance()->commandSelectionChanged(currentUserCommand);
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
	LOG("here ?");
	LOG(cliActionType->getValue().toString());
	if (cliActionType->getValue() != "") {
		txts = getCliAsTexts();
	}
	else if (currentUserCommand == nullptr) {return ""; }
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

StringArray Programmer::getCliAsTexts() {
	StringArray words;
	userCantPress();
	userCanPressAction = true;
	currentUserTarget = nullptr;
	String action = cliActionType->getValue();
	if (action != "") {
		words.add(cliActionType->getValue());
		userCanPressTargetType = true;
		currentUserTarget = cliParamAType;

		if (cliParamAType->getValue() != "") {
			words.add(cliParamAType->getValueData());
			userCanPressAction = false;
			userCanPressNumber = true;
			currentUserTarget = cliParamAId;

			if ((int)cliParamAId->getValue() > 0) {
				words.add(cliParamAId->getValue());
				// value entered
				if (action == "copy" || action == "move") {
					userCanPressTargetType = true;
					currentUserTarget = cliParamBType;

					if (cliParamBType->getValue() != "") {
						words.add(cliParamBType->getValueData());
						userCanPressTargetType = false;
						currentUserTarget = cliParamBId;
						if ((int)cliParamAId->getValue() > 0) {
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



	for (int i = 0; i < words.size(); i++) {
		LOG(words[i]);
	}
	return words;

}

void Programmer::userCantPress() {
	userCanPressAction = false;
	userCanPressTargetType = false;
	userCanPressNumber = false;
	userCanPressGo = false;

}

