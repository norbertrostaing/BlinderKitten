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
	channels(),
	chansManager()
{
	saveAndLoadRecursiveData = true;
	editorIsCollapsed = true;

	itemDataType = "DeviceType";
	
	addStringParameter("Manufacturer", "Name of the Manufacturer", "");
	addStringParameter("User infos", "Free text for you", "");

	updateChannels = addTrigger("Update channels numbers", "Yay, it should be automatic, but I'm not that good");

	chansManager = new BaseManager<DeviceTypeChannel>("Channels");
	chansManager -> selectItemWhenCreated = false;
	// ContainerAsyncListener* newListener = new ContainerAsyncListener();
	// chansManager->addAsyncContainerListener();
	channels.reset(chansManager);
	addChildControllableContainer(channels.get());
}

DeviceType::~DeviceType()
{
}

void DeviceType::triggerTriggered(Trigger* t) {
	if (t == updateChannels) {
		calcDmxChannels();
	}
}

void DeviceType::calcDmxChannels() {
	Array<DeviceTypeChannel*> chans = channels->getItemsWithType<DeviceTypeChannel>();
	int current = 1; 
	for (int i = 0; i < chans.size(); i++) {
		chans[i]->dmxDelta->setValue(current);
		if (chans[i]->resolution->getValue() == "8bits") {
			current += 1;
		}
		else if(chans[i]->resolution->getValue() == "16bits") {
			current += 2;
		}
		else {
			LOG("no resolution !");
		}
	}
}

void DeviceType::onContainerParameterChangedInternal(Parameter* p) {
	LOG("coucou");
}