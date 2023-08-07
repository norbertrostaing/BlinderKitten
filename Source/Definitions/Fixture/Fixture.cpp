/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "Fixture.h"
#include "FixturePatch.h"
#include "FixtureManager.h"
#include "../../Brain.h"
#include "../FixtureType/FixtureTypeManager.h"
#include "../FixtureType/FixtureType.h"
#include "../SubFixture/SubFixture.h"
#include "../SubFixture/SubFixtureChannel.h"
#include "../ChannelFamily/ChannelFamily.h"
#include "FixtureMultiEditor.h"
#include "UI/GridView/FixtureGridView.h"
#include "BKEngine.h"


Fixture::Fixture(var params) :
	BaseItem(params.getProperty("name", "Fixture")),
	objectType(params.getProperty("type", "Fixture").toString()),
	objectData(params),
	devTypeParam(),
	patchs("Patch"),
	subFixtures()
{
	nameCanBeChangedByUser = false;
	saveAndLoadRecursiveData = true;
	editorIsCollapsed = true;
	canBeDisabled = false;

	itemDataType = "Fixture";
	
	id = addIntParameter("ID", "ID of this Fixture", 1, 1);
	userName = addStringParameter("Name", "Name of this Fixture", "New Fixture");
	updateName();

	devTypeParam = addTargetParameter("Fixture type", "Type of Fixture", FixtureTypeManager::getInstance());
	devTypeParam -> targetType = TargetParameter::CONTAINER;
	devTypeParam -> maxDefaultSearchLevel = 0;
	
	// to add a manager with defined data
	patchs.selectItemWhenCreated = false;
	addChildControllableContainer(&patchs);

	//Logger::writeToLog("call from constructor");
	Brain::getInstance()->registerFixture(this, id->getValue());
	checkChildrenSubFixtures();

	
}

void Fixture::afterLoadJSONDataInternal() {
	// Logger::writeToLog("call from JSONDataInternal");
	checkChildrenSubFixtures();
}

Fixture::~Fixture()
{
	Brain::getInstance()->unregisterFixture(this);
	subFixturesContainer.clear();
	if (!BKEngine::mainEngine->isClearing) {
		FixtureGridView::getInstance()->updateCells();
	}
	
}


void Fixture::onContainerNiceNameChanged() 
{
	BaseItem::onContainerNiceNameChanged();
	// Logger::writeToLog("call from nameChanged");
	checkChildrenSubFixtures();
}

void Fixture::onContainerParameterChangedInternal(Parameter* p) 
{
	if (p == userName || p == id) {
		updateName();
		FixtureGridView::getInstance()->updateCells();
	}
	if (p == devTypeParam)
	{
		// Logger::writeToLog("call from ParamChanged");
		checkChildrenSubFixtures();
		if (userName->getValue().toString() == "New Fixture") {
			autoName();
		}
	}
	else if(p == id) 
	{
		Brain::getInstance()->registerFixture(this, id->getValue(), true);
	}
}

void Fixture::onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c)
{
	if (cc == &patchs) {
		for (auto it = subFixtures.begin(); it != subFixtures.end(); it.next()) {
			SubFixture* sf = it.getValue();
			if (sf != nullptr) {
				for (auto it2 = sf->channelsMap.begin(); it2 != sf->channelsMap.end(); it2.next()) {
					Brain::getInstance()->pleaseUpdate(it2.getValue());
				}
			}
		}
	}
}

void Fixture::applyPatchs() {
	if (devTypeParam == nullptr) { return; }

}

void Fixture::checkChildrenSubFixtures() {
	if (devTypeParam == nullptr) { return; }

	const MessageManagerLock mmLock;

	subFixturesContainer.clear();
	subFixtures.clear();

	FixtureType* t = dynamic_cast<FixtureType*>(devTypeParam->targetContainer.get());
	if (t== nullptr) {
		return ;
	}

	//Array<WeakReference<ControllableContainer>> chans = t->chansManager.getAllContainers();

	// clean of unused SubFixtures
	// should probabely clean subfixture brain updates

	// virtual channels !
	for (int i = 0; i < t->virtualChansManager.items.size(); i++) {
		FixtureTypeVirtualChannel* c = t->virtualChansManager.items[i];
		int subId = c->subFixtureId->getValue();
		SubFixture* subFixt = subFixtures.getReference(subId);
		if (subFixt == nullptr) {
			subFixt = new SubFixture();
			subFixturesContainer.add(subFixt);
			subFixtures.set(subId, subFixt);
			subFixt->subId = subId;
		}

		if (subFixt != nullptr && subFixt->parentFixture != this) {
			subFixt->parentFixture = this;
		}

		if (subFixt != nullptr && c != nullptr) {
			ChannelType* param = dynamic_cast<ChannelType*>(c->channelType->targetContainer.get());
			if (param != nullptr) {
				if (subFixt->channelsMap.contains(param)) {
					LOGERROR("You have multiple channels with the same type in the same subfixture !");
				}
				SubFixtureChannel* chan = new SubFixtureChannel();
				subFixt->channelsContainer.add(chan);
				subFixt->channelsMap.set(param, chan);
				chan->defaultValue = c->defaultValue->getValue();
				chan->highlightValue = c->highlightValue->getValue();
				chan->isHTP = param->priority->getValue() == "HTP";
				chan->channelType = dynamic_cast<ChannelType*>(c->channelType->targetContainer.get());
				chan->parentParamDefinition = param;
				chan->snapOnly = c->fadeOrSnap->getValue().toString() == "snap";
				chan->parentFixtureTypeChannel = nullptr;
				chan->parentFixture = this;
				chan->parentSubFixture = subFixt;
				chan->subFixtureId = subId;

				if (param->reactGM->getValue()) {
					chan->reactToGrandMaster = true;
					Brain::getInstance()->grandMasterChannels.add(chan);
				}

				if (c->killedBySWOP->getValue()) {
					chan->swopKillable = true;
					Brain::getInstance()->swoppableChannels.add(chan);
				}
				Brain::getInstance()->pleaseUpdate(chan);
			}
		}
	}

	// DMX Channels
	for (int i = 0; i < t->chansManager.items.size(); i++) {
		FixtureTypeChannel* c = t->chansManager.items[i];
		int subId = c->subFixtureId->getValue();
		SubFixture* subFixt = subFixtures.contains(subId) ? subFixtures.getReference(subId) : nullptr;
		if (subFixt == nullptr) {
			subFixt = new SubFixture();
			subFixturesContainer.add(subFixt);
			subFixtures.set(subId, subFixt);
			subFixt->subId = subId;
		}

		if (subFixt != nullptr && subFixt->parentFixture != this) {
			subFixt->parentFixture = this;
		}

		if (subFixt != nullptr && c != nullptr) {
			ChannelType* param = dynamic_cast<ChannelType*>(c->channelType->targetContainer.get());
			if (param != nullptr) {
				if (subFixt->channelsMap.contains(param)) {
					LOGERROR("You have multiple channels with the same type in the same subfixture ! ("+String(subId)+" - "+param->niceName+" )");
				}
				SubFixtureChannel* chan = new SubFixtureChannel();
				subFixt->channelsContainer.add(chan);
				subFixt->channelsMap.set(param, chan);
				chan->defaultValue = c->defaultValue->getValue();
				chan->highlightValue = c->highlightValue->getValue();
				chan->isHTP = param->priority->getValue() == "HTP";
				chan->resolution = c->resolution->getValue();
				chan->channelType = dynamic_cast<ChannelType*>(c->channelType->targetContainer.get());
				chan->parentParamDefinition = param;
				chan->snapOnly = c->fadeOrSnap->getValue().toString() == "snap";
				chan->parentFixtureTypeChannel = c;
				chan->parentFixture = this;
				chan->parentSubFixture = subFixt;
				chan->subFixtureId = subId;
				chan->invertOutput = c->invertOutput->boolValue();

				if (param->reactGM->getValue()) {
					chan->reactToGrandMaster = true;
					Brain::getInstance()->grandMasterChannels.add(chan);
				}

				if (c->killedBySWOP->getValue()) {
					chan->swopKillable = true;
					Brain::getInstance()->swoppableChannels.add(chan);
				}

				FixtureTypeVirtualChannel* virtualMaster = dynamic_cast<FixtureTypeVirtualChannel *>(c->virtualMaster->targetContainer.get());

				if (virtualMaster != nullptr) {
					int masterSubFixtureId = virtualMaster->subFixtureId->getValue();
					ChannelType* masterType = dynamic_cast<ChannelType*>(virtualMaster->channelType->targetContainer.get());
					SubFixtureChannel* master = subFixtures.getReference(masterSubFixtureId)->channelsMap.getReference(masterType);
					if (master != nullptr) {
						master->virtualChildren.add(chan);
						
						chan->virtualMaster = master;
					}
				}

				Brain::getInstance()->pleaseUpdate(chan);
			}
		}
	}

}

void Fixture::updateName() {
	String n = userName->getValue();
	if (parentContainer != nullptr) {
		dynamic_cast<FixtureManager*>(parentContainer.get())->reorderItems();
	}
	setNiceName(String((int)id->getValue()) + " - " + n);
}


Array<SubFixture*> Fixture::getAllSubFixtures() {
	Array<SubFixture*> ret;
	for (auto it = subFixtures.begin(); it != subFixtures.end(); it.next()) {
		if (it.getValue() != nullptr) {
			ret.add(it.getValue());
		}
	}
	return ret;
}

SubFixture* Fixture::getSubFixture(int subId) {
	return subFixtures.getReference(subId);
}


void Fixture::selectThis(bool addToSelection, bool notify) {
	BaseItem::selectThis(addToSelection, notify);
	if (addToSelection) {
		InspectorUI* inspectorUI = dynamic_cast<InspectorUI*>(ShapeShifterManager::getInstance()->getContentForName("Inspector")->contentComponent);
		// multi fixture edition disabled organic ui update
		auto currentSel = selectionManager->currentInspectables;
		Array<Inspectable*> newSel;
		newSel.addArray(currentSel);
		if (!newSel.contains(FixtureMultiEditor::getInstance())) {
			newSel.insert(0,FixtureMultiEditor::getInstance());
		}
		inspectorUI->inspector->setCurrentInspectables(newSel);
	}
}

void Fixture::autoName() {
	FixtureType* ft = dynamic_cast<FixtureType*>(devTypeParam->targetContainer.get());
	if (ft!=nullptr) {
		userName->setValue(ft->niceName);
	}
}

