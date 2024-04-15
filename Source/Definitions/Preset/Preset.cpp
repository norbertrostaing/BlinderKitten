/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "Preset.h"
#include "PresetValue.h"
#include "Definitions/ChannelFamily/ChannelFamilyManager.h"
#include "../ChannelFamily/ChannelType/ChannelType.h"
#include "../../Brain.h"
#include "PresetManager.h"
#include "UI/GridView/PresetGridView.h"
#include "UserInputManager.h"
#include "Definitions/DataTransferManager/DataTransferManager.h"

int comparePresetContent(PresetSubFixtureValues* A, PresetSubFixtureValues* B) {
	if (A->targetFixtureId->getValue() == B->targetFixtureId->getValue()) {
		return (int)A->targetSubFixtureId->getValue() - (int)B->targetSubFixtureId->getValue();
	}
	else {
		return (int)A->targetFixtureId->getValue() - (int)B->targetFixtureId->getValue();
	}
}

Preset::Preset(var params) :
	BaseItem(params.getProperty("name", "Preset")),
	objectType(params.getProperty("type", "Preset").toString()),
	objectData(params),
	subFixtureValues("Subfixtures"),
	devTypeParam()
{
	saveAndLoadRecursiveData = true;
	editorIsCollapsed = true;
	nameCanBeChangedByUser = false;
	canBeDisabled = false;

	itemDataType = "Preset";
	id = addIntParameter("ID", "ID of this Preset", 1, 1);
	userName = addStringParameter("Name", "Name of this preset", "New preset");
	updateName();

	String presetExplain = "Behaviour of the preset : \n\n";
	presetExplain += "- SubFixture : preset applyes only to recorded SubFixtures\n\n";
	// presetExplain += "- SubFixture Type : preset applies only to SubFixtures who have the same SubFixture type (same SubFixture suffix in Fixture type)\n\n";
	presetExplain += "- Fixture type : preset applies only to SubFixtures from the same Fixture type\n\n";
	presetExplain += "- Same channels : preset applies to all SubFixtures with same channels\n\n";
	
	//loadToProgrammerBtn = addTrigger("Load", "Load fixtures values in the programmer (values with fixture ID 0 are not called)");
	testMeButton = addTrigger("Test me", "call any fixture possible with this preset");
	reOrderButton = addTrigger("Reorder Data", "Re arrange preset data");

	presetType = addEnumParameter("Type", presetExplain);
	presetType->addOption("SubFixture", 1);
	// presetType->addOption("SubFixture Type", 2);
	presetType->addOption("Fixture Type", 3);
	presetType->addOption("Same Channels type", 4);

	// to add a manager with defined data
	subFixtureValues.selectItemWhenCreated = false;
	subFixtureValues.comparator.compareFunc = comparePresetContent;
	addChildControllableContainer(&subFixtureValues);

	Brain::getInstance()->registerPreset(this, id->getValue());
	if (!Engine::mainEngine->isLoadingFile) {
		PresetGridView::getInstance()->updateCells();
	}
	if (params.isVoid()) {
		subFixtureValues.addItem();
	}

}

Preset::~Preset()
{
	Brain::getInstance()->unregisterPreset(this);
	for (auto it = computedSubFixtureValues.begin(); it != computedSubFixtureValues.end(); it.next()) {
		//it.getValue()->~HashMap();
	}
	for (auto it = computedFixtureTypeValues.begin(); it != computedFixtureTypeValues.end(); it.next()) {
		//it.getValue()->~HashMap();
	}
	for (auto it = computedSubFixtureTypeValues.begin(); it != computedSubFixtureTypeValues.end(); it.next()) {
		for (auto it2 = it.getValue()->begin(); it2 != it.getValue()->end(); it2.next()) {
			//it2.getValue()->~HashMap();
		}
		//it.getValue()->~HashMap();
	}
	PresetGridView::getInstance()->updateCells();
}

void Preset::onContainerParameterChangedInternal(Parameter* p) {
	if (p == id) {
		Brain::getInstance()->registerPreset(this, id->getValue(), true);
	}
	if (p == userName || p == id) {
		updateName();
	}
	if (p == presetType) {
		checkIfProgrammerNeedUpdate();
	}
	PresetGridView::getInstance()->updateCells();
}


void Preset::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) {
	checkIfProgrammerNeedUpdate();
}

void Preset::checkIfProgrammerNeedUpdate()
{
	Programmer* currentProgrammer = UserInputManager::getInstance()->getProgrammer(false);
	bool programmerNeedRefresh = false;
	int myId = id->getValue();
	if (currentProgrammer != nullptr) {
		for (int iCommand = 0; iCommand < currentProgrammer->commands.items.size() && !programmerNeedRefresh; iCommand++) {
			Command* currentCommand = currentProgrammer->commands.items[iCommand];
			for (int iValue = 0; iValue < currentCommand->values.items.size() && !programmerNeedRefresh; iValue++) {
				CommandValue* cv = currentCommand->values.items[iValue];
				if (cv->presetOrValue->getValue() == "preset") {
					int tFromId = cv->presetIdFrom->getValue();
					int tToId = cv->presetIdTo->getValue();
					if (tFromId == myId) { programmerNeedRefresh = true; }
					if (tToId == myId) { programmerNeedRefresh = true; }
				}
			}

		}
		if (programmerNeedRefresh) {
			Brain::getInstance()->pleaseUpdate(currentProgrammer);
		}
	}
}

void Preset::computeValues() {
	computedSubFixtureValues.clear();
	computedFixtureTypeValues.clear();
	computedSubFixtureTypeValues.clear();
	computedUniversalValues.clear();

	Array<PresetSubFixtureValues*> fixtValues = subFixtureValues.getItemsWithType<PresetSubFixtureValues>();
	int pType = presetType->getValue();

	for (int commandIndex = 0; commandIndex < fixtValues.size(); commandIndex++) {
		PresetSubFixtureValues* fixtVal = fixtValues[commandIndex];
		Fixture* fixt = nullptr;
		SubFixture* subFixt = nullptr;
		FixtureType* type = nullptr;
		int fixtureId = fixtVal->targetFixtureId->getValue();
		int subFixtId = fixtVal->targetSubFixtureId->getValue();
		if (fixtureId > 0) {
			fixt = Brain::getInstance()->getFixtureById(fixtureId);
			if (fixt != nullptr) {
				type = dynamic_cast<FixtureType*>(fixt -> devTypeParam -> targetContainer.get());
				subFixt = fixt->subFixtures.getReference(subFixtId);
			}
		}
		
		Array<PresetValue *> values = fixtVal->values.getItemsWithType<PresetValue>();
		for (int valIndex = 0; valIndex < values.size(); valIndex++) {
			ChannelType* param = dynamic_cast<ChannelType*>(values[valIndex]->param->targetContainer.get());
			float value = values[valIndex] -> paramValue -> getValue();
			if (param != nullptr) {
				if (pType >= 1 && subFixt != nullptr) {
					std::shared_ptr<HashMap<ChannelType*, float>> content = computedSubFixtureValues.getReference(subFixt);
					if (content == nullptr) {
						content = std::make_shared<HashMap<ChannelType*, float>>();
						computedSubFixtureValues.set(subFixt, content);
					}
					content -> set(param, value); 
				}
				// Fixture mode
				if (pType >= 3 && type != nullptr) {
					std::shared_ptr<HashMap<ChannelType*, float>> content = computedFixtureTypeValues.getReference(type);
					if (content == nullptr) {
						content = std::make_shared<HashMap<ChannelType*, float>>();
						computedFixtureTypeValues.set(type, content);
					}
					content->set(param, value);
				}

				// same param mode
				if (pType >= 4) {
					if (!computedUniversalValues.contains(param)) {
						computedUniversalValues.set(param, value);
					}
				}

			}

		}


	}

}

	std::shared_ptr < HashMap<ChannelType*, float>> Preset::getSubFixtureValues(SubFixture* f) {
	std::shared_ptr < HashMap<ChannelType*, float>> values = std::make_shared<HashMap<ChannelType*, float>>();
	for (auto it = computedUniversalValues.begin(); it != computedUniversalValues.end(); it.next()) {
		values->set(it.getKey(), it.getValue());
	}

	FixtureType* dt = dynamic_cast<FixtureType*>(f->parentFixture->devTypeParam->targetContainer.get());
	if (computedFixtureTypeValues.contains(dt)) {
		std::shared_ptr<HashMap<ChannelType*, float>> fixtureTypeValues = computedFixtureTypeValues.getReference(dt);
		for (auto it = fixtureTypeValues->begin(); it != fixtureTypeValues->end(); it.next()) {
			values->set(it.getKey(), it.getValue());
		}
	}

	if (computedSubFixtureValues.contains(f)) {
		std::shared_ptr<HashMap<ChannelType*, float>> currentSubFixtureValues = computedSubFixtureValues.getReference(f);
		for (auto it = currentSubFixtureValues->begin(); it != currentSubFixtureValues->end(); it.next()) {
			values->set(it.getKey(), it.getValue());
		}
	}

	return values;
}

void Preset::triggerTriggered(Trigger* t)
{
	if (t == loadToProgrammerBtn) {
		DataTransferManager::getInstance()->sourceType->setValueWithData("preset");
		DataTransferManager::getInstance()->sourceId->setValue(id->getValue());
		DataTransferManager::getInstance()->targetType->setValueWithData("programmer");
		DataTransferManager::getInstance()->targetUserId->setValue(1);
		DataTransferManager::getInstance()->selectThis();
	}
	if (t == testMeButton) {
		UserInputManager::getInstance()->testPreset(this);
	}
	if (t == reOrderButton) {
		ChannelFamilyManager::getInstance()->updateOrderedElements();
		reorderPresetContent();
	}
}


void Preset::updateName() {
	String n = userName->getValue();
	if (parentContainer != nullptr) {
		dynamic_cast<PresetManager*>(parentContainer.get())->reorderItems();
	}
	setNiceName(String((int)id->getValue()) + " - " + n);
}



void Preset::updateDisplay()
{
	queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

void Preset::reorderPresetContent() {
	subFixtureValues.reorderItems();
	for (int i = 0; i < subFixtureValues.items.size(); i++) {
		subFixtureValues.items[i]->values.reorderItems();
	}
}