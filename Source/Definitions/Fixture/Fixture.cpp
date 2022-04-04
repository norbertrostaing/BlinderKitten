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


Fixture::Fixture(var params) :
	BaseItem(params.getProperty("name", "Fixture")),
	objectType(params.getProperty("type", "Fixture").toString()),
	objectData(params),
	patchs("Patch"),
	devTypeParam(),
	subFixtures()
{
	nameCanBeChangedByUser = false;
	saveAndLoadRecursiveData = true;
	editorIsCollapsed = true;

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
	for (auto it = subFixtures.begin(); it != subFixtures.end(); it.next()) {
		if (it.getValue()!= nullptr) {
			//it.getValue()->~SubFixture();
			delete it.getValue();
		}
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

void Fixture::applyPatchs() {
	if (devTypeParam == nullptr) { return; }

}

void Fixture::checkChildrenSubFixtures() {
	if (devTypeParam == nullptr) { return; }

	FixtureType* t = dynamic_cast<FixtureType*>(devTypeParam->targetContainer.get());
	if (t== nullptr) {
		return ;
	}

	Array<WeakReference<ControllableContainer>> chans = t->chansManager.getAllContainers();
	//Array<WeakReference<FixtureTypeChannel*>> chans = t->chansManager.getItemsWithType<FixtureTypeChannel*>();
	Array<String> fixtNames;
	// HashMap<String, Array<String>> fixtChannels;

	// clean of unused SubFixtures
	// should probabely clean subfixture brain updates
	for (auto it = subFixtures.begin(); it != subFixtures.end(); it.next()) {
		//it.getValue()->~SubFixture();
		delete it.getValue();
	}
	subFixtures.clear();

	for (int i = 0; i < chans.size(); i++) {
		FixtureTypeChannel* c = dynamic_cast<FixtureTypeChannel*>(chans[i].get());
		int subId = c->subFixtureId->getValue();
		SubFixture* subFixt = subFixtures.getReference(subId);
		if (subFixt == nullptr) {
			subFixt = new SubFixture();
			subFixtures.set(subId, subFixt);
			subFixt->subId = subId;
		}
		
		if (subFixt != nullptr && subFixt->parentFixture != this) {
			subFixt->parentFixture = this;
		}

		if (subFixt != nullptr && c != nullptr) {
			ChannelType* param = dynamic_cast<ChannelType*>(c -> channelType -> targetContainer.get());
			if (param != nullptr) {
				if (subFixt->channelsMap.contains(param)) {
					LOGERROR("You have multiple channels with the same type in the same subfixture !");
				}
				SubFixtureChannel* chan = new SubFixtureChannel();
				subFixt->channelsMap.set(param, chan);
				chan -> defaultValue = c->defaultValue->getValue();
				chan -> isHTP = param->priority->getValue() == "HTP";
				chan -> resolution = c->resolution->getValue();
				chan -> channelType = dynamic_cast<ChannelType*>(c->channelType->targetContainer.get());
				chan -> parentParamDefinition = param;
				chan -> snapOnly = c->fadeOrSnap->getValue().toString() == "snap";
				chan -> parentFixtureTypeChannel = c;
				chan -> parentFixture = this;
				chan -> parentSubFixture = subFixt;
				if (c->killedBySWOP->getValue()) {
					chan->swopKillable = true;
					Brain::getInstance()->swoppableChannels.add(chan);
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

SubFixture* Fixture::getSubFixture(int id) {
	return subFixtures.getReference(id);
}


void Fixture::selectThis(bool addToSelection, bool notify) {
	BaseItem::selectThis(addToSelection, notify);
	if (addToSelection) {
		InspectorUI* inspectorUI = dynamic_cast<InspectorUI*>(ShapeShifterManager::getInstance()->getContentForName("Inspector")->contentComponent);
		inspectorUI->inspector->setCurrentInspectable(FixtureMultiEditor::getInstance());
		// inspectorUI->inspector->vp.setViewedComponent(FixtureMultiEditor::getInstance()->getEditor(true), false);
	}
}

void Fixture::autoName() {
	FixtureType* ft = dynamic_cast<FixtureType*>(devTypeParam->targetContainer.get());
	if (ft!=nullptr) {
		userName->setValue(ft->niceName);
	}
}

