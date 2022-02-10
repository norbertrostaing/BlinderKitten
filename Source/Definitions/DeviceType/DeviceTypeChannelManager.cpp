#include "DeviceTypeChannelManager.h"
#include "../../Brain.h"

DeviceTypeChannelManager::DeviceTypeChannelManager() :
    BaseManager("Channels")
{
    itemDataType = "DeviceTypeChannel";
    selectItemWhenCreated = false;       
}

DeviceTypeChannelManager::~DeviceTypeChannelManager()
{
}

void DeviceTypeChannelManager::calcDmxChannels() {
	int current = 1;
	for (int i = 0; i < items.size(); i++) {
		items[i]->dmxDelta->setValue(current);
		items[i]->setNiceName("Channel "+String(current));
		if (items[i]->resolution->getValue() == "8bits") {
			current += 1;
		}
		else if (items[i]->resolution->getValue() == "16bits") {
			current += 2;
		}
		else {
			LOG("no resolution !");
		}
	}
}



/*
void DeviceTypeChannelManager::askForMoveBefore(BaseItem* c) {
    BaseManager::askForMoveBefore(c);
};
void DeviceTypeChannelManager::askForMoveAfter(BaseItem* c) {
    BaseManager::askForMoveAfter(c);
};
void DeviceTypeChannelManager::addItemInternal(DeviceTypeChannel* c, var data) {
};

void DeviceTypeChannelManager::askForMoveAfter(BaseItem* c) {
    BaseManager::askForMoveAfter(c);
};
void DeviceTypeChannelManager::askForMoveAfter(BaseItem* c) {
    BaseManager::askForMoveAfter(c);
};
*/

void DeviceTypeChannelManager::addItemInternal(DeviceTypeChannel* c, var data) {
	calcDmxChannels();
}
void DeviceTypeChannelManager::askForRemoveBaseItem(BaseItem* item) {
	BaseManager::askForRemoveBaseItem(item);
	calcDmxChannels();
}
void DeviceTypeChannelManager::askForDuplicateItem(BaseItem* item) {
	BaseManager::askForDuplicateItem(item);
	calcDmxChannels();
}
void DeviceTypeChannelManager::askForPaste() {
	BaseManager::askForPaste();
	calcDmxChannels();
}
void DeviceTypeChannelManager::askForMoveBefore(BaseItem* i) {
	BaseManager::askForMoveBefore(i);
	calcDmxChannels();
}
void DeviceTypeChannelManager::askForMoveAfter(BaseItem* i) {
	BaseManager::askForMoveAfter(i);
	calcDmxChannels();
}
