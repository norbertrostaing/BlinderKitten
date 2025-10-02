/*
  ==============================================================================

    FixtureTypeDMXChannelManager.cpp
    Created: 1 Oct 2025
    Author:  j-mutter

  ==============================================================================
*/

#include "FixtureTypeDMXChannelManager.h"
#include "../../Brain.h"

FixtureTypeDMXChannelManager::FixtureTypeDMXChannelManager() :
    BaseManager("DMX Channels")
{
    itemDataType = "FixtureTypeDMXChannel";
    selectItemWhenCreated = false;
}

FixtureTypeDMXChannelManager::~FixtureTypeDMXChannelManager()
{
}

void FixtureTypeDMXChannelManager::calcDmxChannels()
{
    int current = 1;
    for (int i = 0; i < items.size(); i++) {
        items[i]->setNiceName("temp "+String(i));
    }
    for (int i = 0; i < items.size(); i++) {
        items[i]->dmxDelta->setValue(current);
        String name = "Channel " + String(current);
        if (items[i]->resolution->getValue() == "8bits") {
            current += 1;
        }
        else if (items[i]->resolution->getValue() == "fine") {
            current += 1;
        }
        else if (items[i]->resolution->getValue() == "16bits") {
            name += " + "+String(current+1);
            current += 2;
        }
        else {
            LOG("no resolution !");
        }
        
        items[i]->setNiceName(name);
    }
}

void FixtureTypeDMXChannelManager::addItemInternal(FixtureTypeDMXChannel* c, var data) {
    calcDmxChannels();
    if (parentContainer != nullptr && !Brain::getInstance()->loadingIsRunning) {
        FixtureType* ft = dynamic_cast<FixtureType* >(parentContainer.get());
        ft->updateVirtualLists();
    }

}
void FixtureTypeDMXChannelManager::askForRemoveBaseItem(BaseItem* item) {
    BaseManager::askForRemoveBaseItem(item);
    calcDmxChannels();
}
void FixtureTypeDMXChannelManager::askForDuplicateItem(BaseItem* item) {
    BaseManager::askForDuplicateItem(item);
    calcDmxChannels();
}
void FixtureTypeDMXChannelManager::askForPaste() {
    BaseManager::askForPaste();
    calcDmxChannels();
}
void FixtureTypeDMXChannelManager::askForMoveBefore(BaseItem* i) {
    BaseManager::askForMoveBefore(i);
    calcDmxChannels();
}
void FixtureTypeDMXChannelManager::askForMoveAfter(BaseItem* i) {
    BaseManager::askForMoveAfter(i);
    calcDmxChannels();
}

void FixtureTypeDMXChannelManager::setItemIndex(FixtureTypeDMXChannel* item, int newIndex, bool addToUndo)
{
    BaseManager::setItemIndex(item, newIndex, addToUndo);
    calcDmxChannels();
}
