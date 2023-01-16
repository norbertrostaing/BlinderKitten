/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "FixtureType.h"
#include "Brain.h"

FixtureType::FixtureType(var params) :
	BaseItem(params.getProperty("name", "FixtureType")),
	objectType(params.getProperty("type", "FixtureType").toString()),
	objectData(params),
	chansManager(),
	virtualChansManager()
{
	saveAndLoadRecursiveData = true;
	editorIsCollapsed = true;
	canBeDisabled = false;

	itemDataType = "FixtureType";
	
	addStringParameter("Manufacturer", "Name of the Manufacturer", "");
	addStringParameter("User infos", "Free text for you", "");

	//chansManager = new BaseManager<FixtureTypeChannel>("Channels");
	// ContainerAsyncListener* newListener = new ContainerAsyncListener();
	// chansManager->addAsyncContainerListener();
	addChildControllableContainer(&chansManager);
	addChildControllableContainer(&virtualChansManager);
}

FixtureType::~FixtureType()
{
}

void FixtureType::afterLoadJSONDataInternal() {
	updateVirtualLists();
}

void FixtureType::updateVirtualLists() {
	for (int i = 0; i < chansManager.items.size(); i++) {
		String value = chansManager.items[i]->virtualMaster->getValue();
		chansManager.items[i]->virtualMaster->setRootContainer(&virtualChansManager);
		chansManager.items[i]->virtualMaster->setValue(value);
	}

}
