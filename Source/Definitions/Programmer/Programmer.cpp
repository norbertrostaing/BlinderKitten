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
#include "UI/Encoders.h"
#include "UI/InputPanel.h"
#include "UI/EncodersMult/EncodersMult.h"

Programmer::Programmer(var params) :
	BaseItem(params.getProperty("name", "Programmer")),
	objectType(params.getProperty("type", "Programmer").toString()),
	objectData(params),
	commands("Commands"),
	cliContainer("Command Line"),
	timing()
{
	saveAndLoadRecursiveData = true;
	nameCanBeChangedByUser = false;
	editorIsCollapsed = false;
	canBeDisabled = false;
	itemDataType = "Programmer";

	cliContainer.editorIsCollapsed = true;

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
	cleanUnusedCommandsBtn = addTrigger("Clean", "Clean the duplicates commands in this cue.");

	highlightCurrentCommand = addBoolParameter("Highlight", "Highlite the current command", false);

	cliActionType = cliContainer.addEnumParameter("Action type", "What kind of action do you wanna do ?");
	cliActionType->addOption("None", "");
	cliActionType->addOption("Copy", "copy");
	cliActionType->addOption("Move", "move");
	cliActionType->addOption("Record", "record");
	cliActionType->addOption("Merge", "merge");
	cliActionType->addOption("Replace", "replace");
	cliActionType->addOption("Edit", "edit");
	cliActionType->addOption("Load content", "loadcontent");
	cliActionType->addOption("Delete", "delete");
	cliActionType->addOption("Set Another", "setanother");

	cliParamAType = cliContainer.addEnumParameter("Param A type", "What kind of object do you want to target");
	cliParamAType->addOption("None", "");
	cliParamAType->addOption("Fixture", "fixture");
	cliParamAType->addOption("Group", "group");
	cliParamAType->addOption("Preset", "preset");
	cliParamAType->addOption("Cuelist", "cuelist");
	cliParamAType->addOption("Effect", "effect");
	cliParamAType->addOption("Carousel", "carousel");
	cliParamAType->addOption("Mapper", "mapper");
	cliParamAType->addOption("Virtual button", "virtualbutton");
	cliParamAType->addOption("Virtual fader column", "virtualfadercol");
	cliParamAType->addOption("Virtual fader element", "virtualfaderelement");
	cliParamAId = cliContainer.addIntParameter("Param A ID", "ID of first param", 0, 0);

	cliParamBType = cliContainer.addEnumParameter("Param B type", "second object type");
	cliParamBType->addOption("None", "");
	cliParamBType->addOption("Fixture", "fixture");
	cliParamBType->addOption("Group", "group");
	cliParamBType->addOption("Preset", "preset");
	cliParamBType->addOption("Cuelist", "cuelist");
	cliParamBType->addOption("Effect", "effect");
	cliParamBType->addOption("Carousel", "carousel");
	cliParamBType->addOption("Mapper", "mapper");
	cliParamBType->addOption("Virtual button", "virtualbutton");
	cliParamBType->addOption("Virtual fader column", "virtualfadercol");
	cliParamBType->addOption("Virtual fader element", "virtualfaderelement");
	cliParamBId = cliContainer.addIntParameter("Param B ID", "second object id for copy, move", 0, 0);

	cliGo = cliContainer.addTrigger("GO", "Execute this command");

	timing.presetOrValue->removeOption("Cue timing");
	timing.presetOrValue->setValue("Raw Timing");
	addChildControllableContainer(&timing);

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
	Brain::getInstance()->unregisterProgrammer(this);
	Brain::getInstance()->usingCollections.enter();
	Brain::getInstance()->programmerPoolWaiting.removeAllInstancesOf(this);
	Brain::getInstance()->programmerPoolUpdating.removeAllInstancesOf(this);
	Brain::getInstance()->usingCollections.exit();

	for (auto it = activeValues.begin(); it != activeValues.end(); it.next()) {
		SubFixtureChannel* sfc = it.getKey();
		sfc->programmerOutOfStack(this);
		Brain::getInstance()->pleaseUpdate(sfc);
	}

	if (UserInputManager::getInstance()->getProgrammer(false) == this) {
		UserInputManager::getInstance()->targetCommand = nullptr;
		UserInputManager::getInstance()->currentProgrammer = nullptr;
	}

	if (EncodersMult::getInstanceWithoutCreating() != nullptr) {
		if (EncodersMult::getInstance()->targetCommandManager == &commands) {
			EncodersMult::getInstance()->targetCommandManager = nullptr;
		}
	}


	isDeleted = true;
	clearAll();
}


void Programmer::triggerTriggered(Trigger* t) {
	if (t == goBtn) {
		go();
	}  
	else if (t == releaseBtn) {
		release();
	}
	else if (t == clearAllBtn) {
		clearAll();
	}
	else if (t == recBtn) {
		DataTransferManager::getInstance()->sourceId->setValue(id->getValue());
		DataTransferManager::getInstance()->sourceType->setValue("Programmer");
		DataTransferManager::getInstance()->selectThis();
	}
	else if (t == cleanUnusedCommandsBtn) {
		computeValues();
		cleanUnused();
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
		else {
			go();
		}
		Encoders::getInstance()->updateBlindButton();
		if (UserInputManager::getInstance()->currentProgrammer == this) {
			float v = editionMode->getValue() == "blind" ? 1 : 0;
			UserInputManager::getInstance()->feedback("/blind", v, "");
		}
	}
	if (p == highlightCurrentCommand) {
		go();
		Encoders::getInstance()->updateHLButton();
		if (UserInputManager::getInstance()->currentProgrammer == this) {
			float v = highlightCurrentCommand->boolValue() ? 1 : 0;
			UserInputManager::getInstance()->feedback("/highlight", v, "");
		}
	}

}

void Programmer::computeValues() {
	maxTiming = 0;
	computing.enter();
	computedValues.clear();
	Array<Command*> cs = commands.getItemsWithType<Command>();
	for (int i = 0; i < cs.size(); i++) {
		cs[i]->computeValues(nullptr, nullptr, this);
		maxTiming = std::max(maxTiming, cs[i]->maxTiming);
		cs[i]->isComputing.enter();

		for (auto it = cs[i]->computedValues.begin(); it != cs[i]->computedValues.end(); it.next()) {
			SubFixtureChannel* fc = it.getKey();
			if (fc != nullptr) {
				computedValues.set(fc, it.getValue());
				channelToCommand.set(fc, cs[i]);
			}
		}
		cs[i]->isComputing.exit();
	}

	// Highlights
	if (highlightCurrentCommand->boolValue() && currentUserCommand != nullptr) {
		Array<SubFixture*> subFixtures = currentUserCommand->selection.computedSelectedSubFixtures;
		for (int i = 0; i < subFixtures.size(); i++) {
			SubFixture* sf = subFixtures[i];
            sf->isHighlighted = true;
            if (sf->parentFixture != nullptr) {
                sf->parentFixture->isHighlighted = true;
            }
            Brain::getInstance()->pleaseUpdate(sf);
		}
	}
	else {
		// Clear highlight state when highlight is disabled
		Brain* brain = Brain::getInstance();
		for (Fixture* fixture : brain->fixtures) {
			fixture->isHighlighted = false;
			for (SubFixture* sf : fixture->getAllSubFixtures()) {
				sf->isHighlighted = false;
                Brain::getInstance()->pleaseUpdate(sf);
			}
		}
	}

	computing.exit();
	Brain::getInstance()->layoutViewerNeedRepaint = true;
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
	else if (cc->niceName == "Commands" && c->niceName != "Value" && c->niceName != "Value to")
	{
		UserInputManager::getInstance()->programmerCommandValueChanged(this);
	} else if (c == cliGo) {
		runCliCommand();
	}
	else if (c == cliActionType) {
		InputPanel::getInstance()->updateButtonsText();
	}

}

void Programmer::render(double now) {
	ScopedLock lock(computing);
	release(now);
	computeValues();
	String mode = editionMode->getValue();
	if (mode != "blind") {
		for (auto it = computedValues.begin(); it != computedValues.end(); it.next()) {
			std::shared_ptr<ChannelValue> temp = it.getValue();
			if (activeValues.contains(it.getKey())) {
				temp->values.set(0, it.getKey()->currentValue);
			}
			else {
				temp->values.set(0, -1);
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
	computing.enter();
	Array<SubFixtureChannel*> toUpdate;
	for (auto it = activeValues.begin(); it != activeValues.end(); it.next()) {
		if (it.getValue()->endValue() != -1) {
			std::shared_ptr<ChannelValue> temp = std::make_shared<ChannelValue>();
			float fadeTime = releaseTime->floatValue() * 1000.0f;

			temp->TSInit = now;
			temp->TSStart = now;
			temp->TSEnd = now + fadeTime;

			temp->values.set(1, -1);
			temp->values.set(0, it.getValue()->value);
			temp->isEnded = false;

			temp->fadeCurve = nullptr;

			activeValues.set(it.getKey(), temp);
			toUpdate.add(it.getKey());
		}
	}
	computing.exit();
	for (SubFixtureChannel* sfc : toUpdate) {
		Brain::getInstance()->pleaseUpdate(sfc);
	}
}


float Programmer::applyToChannel(SubFixtureChannel* fc, float currentVal, double now) {
	float val = currentVal;

	bool keepUpdate = false;

	float localValue = 0;
	ScopedLock lock(computing);
	if (!activeValues.contains(fc)) { return currentVal; }
	std::shared_ptr<ChannelValue> cv = activeValues.getReference(fc);

	float valueFrom = currentVal;
	float valueTo = currentVal;
	float outIsOff = false;
	// comportement OK, calcul de valeurs nok

	if (cv->startValue() >= 0) {
		valueFrom = cv->startValue();
	}
	if (cv->endValue() >= 0) {
		valueTo = cv->endValue();
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
		if (cv->fadeCurve != nullptr) {
			fade = cv->fadeCurve->getValueAtPosition(fade);
		}
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
	Brain::getInstance()->virtualFadersNeedUpdate = true;
	if (Encoders::getInstance()->mode != 0) {
		Encoders::getInstance()->mode = 0;
		Encoders::getInstance()->updateModeButton();
	}
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
	Brain::getInstance()->virtualFadersNeedUpdate = true;
	if (Encoders::getInstance()->mode != 0) {
		Encoders::getInstance()->mode = 0;
		Encoders::getInstance()->updateModeButton();
	}

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
		selectCommand(commands.items[index]);
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
		selectCommand(commands.items[index]);
	}
}

void Programmer::selectCommand(Command* c)
{
	checkCurrentUserCommand();
    if (commands.items.indexOf(c) >= 0) {
        currentUserCommand = c;
        if (highlightCurrentCommand->boolValue()) {
            go();
        }
        if (UserInputManager::getInstance()->currentProgrammer == this) {
            auto e = Encoders::getInstance();
            e->updateChannels();
            e->updateEncoders();
            e->updateCommandLine();
        }
	}
}

void Programmer::processUserInput(String s) {
	s = s.toLowerCase();
	const MessageManagerLock mmLock;
	if (s == "clearall") 
		{
		MessageManager::callAsync([this](){
			clearAll();
			});
		}
	else if (cliActionType->getValue() != "") {
		getCliAsTexts();
		String action = cliActionType->getValue();
		if (s.containsOnly("1234567890")) {
			if (userCanPressNumber) {
				String val = currentUserTargetId->getValue().toString();
				val += s;
				currentUserTargetId->setValue(val.getIntValue());
			}
			else {
				LOGERROR(s+ " not allowed");
			}
		}
		else if (s == "fixture" || s == "group" || s == "preset" || s == "cuelist" || s == "effect" || s == "carousel" || s == "mapper" || s == "virtualbutton" || s == "virtualfadercol" || s == "virtualfaderelement") {
			if (userCanPressTargetType) {
				dynamic_cast<EnumParameter*>(currentUserTargetType)->setValueWithData(s);
			}
			else {
				LOGERROR(s+ " not allowed");
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
		else if (s == "clear") {
			cliActionType->setValueWithData(""); 
			cliParamAType->setValueWithData(""); 
			cliParamBType->setValueWithData(""); 
			cliParamAId->setValue(UserInputManager::backspaceOnInt(cliParamAId->getValue())); 
			cliParamBId->setValue(UserInputManager::backspaceOnInt(cliParamBId->getValue())); 
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
				if (action == "record") { cliActionType->setValueWithData("merge"); }
				else if (action == "merge") { cliActionType->setValueWithData("replace"); }
				else { cliActionType->setValueWithData("record"); }
			}
			else {
				LOGERROR(s + " not allowed");
			}
		}
		else if (s == "merge") {
			if (userCanPressAction) {
				cliActionType->setValueWithData("merge");
			}
			else {
				LOGERROR(s + " not allowed");
			}
		}
		else if (s == "replace") {
			if (userCanPressAction) {
				cliActionType->setValueWithData("replace");
			}
			else {
				LOGERROR(s + " not allowed");
			}
		}
		else if (s == "copy") {
			if (userCanPressAction) {
				if (action == "copy") { cliActionType->setValueWithData("move"); }
				else if (action == "move") { cliActionType->setValueWithData("setanother"); }
				else { cliActionType->setValueWithData("copy"); }
			}
			else {
				LOGERROR(s+ " not allowed");
			}
		}
		else if (s == "edit") {
			if (userCanPressAction) {
				if (action == "edit") { cliActionType->setValueWithData("loadcontent"); }
				else { cliActionType->setValueWithData("edit"); }
			}
			else {
				LOGERROR(s+ " not allowed");
			}
		}
		else if (s == "delete") {
			if (userCanPressAction) {
				cliActionType->setValueWithData(s);
			}
			else {
				LOGERROR(s+ " not allowed");
			}
		}

		UserInputManager::getInstance()->updateCommandLine();
		return;
	}

	if (s == "record" || s == "merge" || s == "replace" || s == "copy" || s == "move" || s == "setanother" || s == "edit" || s == "loadcontent" || s == "delete") {
		cliActionType->setValueWithData(s);
		UserInputManager::getInstance()->updateCommandLine();
	}
	checkCurrentUserCommand();
	currentUserCommand->getCommandAsTexts();
	if (s == "clearall") {
		clearAll();
	}
	else if (s == "clear") {
		Brain::getInstance()->pleaseClearProgrammer = true;
	}
	else if (s == "+" || s == "-") {
		if (currentUserCommand->userCanPressPlusOrMinus) {
			currentUserCommand->userPress(s);
		}
		else {
			LOGERROR(s+ " not allowed");
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
		else if (currentUserCommand->userCanPressPlusOrMinus) {
			//currentUserCommand = commands.addItem();
			currentUserCommand->userPress("+");
			currentUserCommand->userPress(s);
		}
		else {
			LOGERROR(s+ " not allowed");
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
			LOGERROR(s+ " not allowed");
		}
	}
	else if (s == "thru") {
		if (currentUserCommand->userCanPressThru) {
			currentUserCommand->userPress(s);
		}
		else {
			LOGERROR(s+ " not allowed");
		}
	}
	else if (s == "preset") {
		if (currentUserCommand->userCanPressValueType) {
			currentUserCommand->userPress(s);
		}
		else {
			LOGERROR(s+ " not allowed");
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
			LOGERROR(s+ " not allowed");
		}
	}
	else {
		// probably a channel type;
	}
	// currentUserCommand->userPress(s);
}

String Programmer::getTextCommand(bool useNames) {
	StringArray txts;
	String txt = "";
	if (cliActionType->getValue() != "") {
		txts = getCliAsTexts();
	}
	else if (currentUserCommand == nullptr) { return ""; }
	else {
		txts = currentUserCommand->getCommandAsTexts(useNames);
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
	if (action == "record" || action == "merge" || action == "replace") {
		DataTransferManager::getInstance()->sourceType->setValueWithData("programmer");
		DataTransferManager::getInstance()->sourceId->setValue(id->getValue());
		DataTransferManager::getInstance()->targetType->setValueWithData(targetType);
		DataTransferManager::getInstance()->targetUserId->setValue(targetId);
		DataTransferManager::getInstance()->groupCopyMode->setValueWithData(action);
		DataTransferManager::getInstance()->presetCopyMode->setValueWithData(action);
		DataTransferManager::getInstance()->cuelistCopyMode->setValueWithData(action == "record" ? "add" : action);
		DataTransferManager::getInstance()->execute();
	}
	else if (action == "copy") {
		DataTransferManager::getInstance()->sourceType->setValueWithData(targetType);
		DataTransferManager::getInstance()->sourceId->setValue(targetId);
		DataTransferManager::getInstance()->targetType->setValueWithData(cliParamBType->getValue());
		DataTransferManager::getInstance()->targetUserId->setValue((int)cliParamBId->getValue());
		DataTransferManager::getInstance()->groupCopyMode->setValueWithData(action == "record" ? "merge" : "replace");
		DataTransferManager::getInstance()->presetCopyMode->setValueWithData("merge");
		DataTransferManager::getInstance()->cuelistCopyMode->setValueWithData(action == "record" ? "add" : "replace");
		DataTransferManager::getInstance()->execute();
	}
	else if (action == "setanother") {
		if (targetType == "fixture") {
			Fixture* f = Brain::getInstance()->getFixtureById(targetId);
			if (f != nullptr) { f->useAnotherId->setValue((int)cliParamBId->getValue()); }
		}
		if (targetType == "group") {
			Group* g = Brain::getInstance()->getGroupById(targetId);
			if (g != nullptr) { g->useAnotherId->setValue((int)cliParamBId->getValue()); }
		}
		if (targetType == "preset") {
			Preset* p = Brain::getInstance()->getPresetById(targetId);
			if (p != nullptr) { p->useAnotherId->setValue((int)cliParamBId->getValue()); }
		}
	}
	else if (action == "loadcontent") {
		Cuelist* c = Brain::getInstance()->getCuelistById(targetId);
		if (c != nullptr) {
			c->loadContent(this);
		}
	}
	else if (action == "move") {
		DataTransferManager::getInstance()->moveObject(targetType, targetId, cliParamBType->getValue(), (int)cliParamBId->getValue());
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
				if (action == "copy" || action == "move" || action == "setanother") {
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
	if (currentUserCommand == nullptr) {
		if (commands.items.size() > 0) {
			currentUserCommand = commands.items[0];
		}
		else {
			currentUserCommand = commands.addItem();
		}
	}
	if (UserInputManager::getInstance()->getProgrammer(false) == this) {
		UserInputManager::getInstance()->targetCommand = currentUserCommand;
	}
}

void Programmer::cleanUnused()
{
	Array<Command*> usedCommands;
	computeValues();
	computing.enter();

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

	computing.exit();
	if (commands.items.size()>0 &&  UserInputManager::getInstance()->currentProgrammer == this) {
		selectCommand(commands.items[0]);
	}
}
