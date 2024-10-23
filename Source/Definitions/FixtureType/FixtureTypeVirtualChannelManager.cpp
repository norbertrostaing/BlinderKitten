#include "FixtureTypeVirtualChannelManager.h"
#include "../../Brain.h"

FixtureTypeVirtualChannelManager::FixtureTypeVirtualChannelManager() :
    BaseManager("Virtual Channels")
{
    itemDataType = "FixtureTypeVirtualChannel";
    selectItemWhenCreated = false;       
}

FixtureTypeVirtualChannelManager::~FixtureTypeVirtualChannelManager()
{
}

void FixtureTypeVirtualChannelManager::addItemInternal(FixtureTypeVirtualChannel* c, var data)
{
    if (parentContainer != nullptr) {
        //parent->updateVirtualLists();
    }
}
