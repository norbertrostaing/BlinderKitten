/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "FixtureType.h"

FixtureType::FixtureType(var params) :
	BaseItem(params.getProperty("name", "FixtureType")),
	objectType(params.getProperty("type", "FixtureType").toString()),
	objectData(params),
	chansManager()
{
	saveAndLoadRecursiveData = true;
	editorIsCollapsed = true;

	itemDataType = "FixtureType";
	
	addStringParameter("Manufacturer", "Name of the Manufacturer", "");
	addStringParameter("User infos", "Free text for you", "");

	//chansManager = new BaseManager<FixtureTypeChannel>("Channels");
	// ContainerAsyncListener* newListener = new ContainerAsyncListener();
	// chansManager->addAsyncContainerListener();
	addChildControllableContainer(&chansManager);
}

FixtureType::~FixtureType()
{
}

