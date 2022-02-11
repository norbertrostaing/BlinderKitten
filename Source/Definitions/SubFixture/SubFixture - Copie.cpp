/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "SubFixture.h"
#include "SubFixtureManager.h"
#include "../../Brain.h"

SubFixture::SubFixture(var params) :
	BaseItem(params.getProperty("name", "SubFixture")),
	objectType(params.getProperty("type", "SubFixture").toString()),
	objectData(params),
	channelsMap()
{
	saveAndLoadRecursiveData = true;
	editorIsCollapsed = true;

	userCanDuplicate = false;
	userCanRemove = false;
	nameCanBeChangedByUser = false;

	itemDataType = "SubFixture";
	Logger::writeToLog("I'm a new SubFixture !");
	
	id = addIntParameter("ID", "ID of this SubFixture",1,1);
	BaseManager<SubFixtureChannel>* m = new BaseManager<SubFixtureChannel>("Channels");
	m->selectItemWhenCreated = false;
	m->selectItemWhenCreated = false;
	m->userCanAddItemsManually = false;
	channelsMap.clear();
	Brain::getInstance()->registerSubFixture(this, id->getValue());
}

SubFixture::~SubFixture()
{
	LOG("killing myself");
	Brain::getInstance()->unregisterSubFixture(this);
}

void SubFixture::onContainerParameterChangedInternal(Parameter* p) {
	Logger::writeToLog("changed !");
	if (p == id) {
		Brain::getInstance()->registerSubFixture(this,id->getValue());
	}
}

