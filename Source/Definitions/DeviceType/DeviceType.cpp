/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "DeviceType.h"

DeviceType::DeviceType(var params) :
	BaseItem(params.getProperty("name", "DeviceType")),
	objectType(params.getProperty("type", "DeviceType").toString()),
	objectData(params),
	chansManager()
{
	saveAndLoadRecursiveData = true;
	editorIsCollapsed = true;

	itemDataType = "DeviceType";
	
	addStringParameter("Manufacturer", "Name of the Manufacturer", "");
	addStringParameter("User infos", "Free text for you", "");

	//chansManager = new BaseManager<DeviceTypeChannel>("Channels");
	// ContainerAsyncListener* newListener = new ContainerAsyncListener();
	// chansManager->addAsyncContainerListener();
	addChildControllableContainer(&chansManager);
}

DeviceType::~DeviceType()
{
}

void DeviceType::onContainerParameterChangedInternal(Parameter* p) {
	LOG("coucou");
}