/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "Fixture.h"
#include "FixtureManager.h"
#include "../../Brain.h"

Fixture::Fixture(var params) :
	BaseItem(params.getProperty("name", "Fixture")),
	objectType(params.getProperty("type", "Fixture").toString()),
	objectData(params),
	channels(),
	channelsMap()
{
	saveAndLoadRecursiveData = true;
	editorIsCollapsed = true;

	userCanDuplicate = false;
	userCanRemove = false;
	nameCanBeChangedByUser = false;

	itemDataType = "Fixture";
	Logger::writeToLog("I'm a new fixture !");
	
	id = addIntParameter("ID", "ID of this Fixture",1,1);
	BaseManager<FixtureChannel>* m = new BaseManager<FixtureChannel>("Channels");
	m->selectItemWhenCreated = false;
	m->selectItemWhenCreated = false;
	m->userCanAddItemsManually = false;
	channels.reset(m);
	channelsMap.clear();
	addChildControllableContainer(channels.get());
	Brain::getInstance()->registerFixture(this, id->getValue());
}

Fixture::~Fixture()
{
	Brain::getInstance()->unregisterFixture(this);
}

void Fixture::onContainerParameterChangedInternal(Parameter* p) {
	Logger::writeToLog("changed !");
	if (p == id) {
		Brain::getInstance()->registerFixture(this,id->getValue());
	}
}

