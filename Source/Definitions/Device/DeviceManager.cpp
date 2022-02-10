#include "Device.h"
#include "DeviceManager.h"

juce_ImplementSingleton(DeviceManager);

int compare(Device* A, Device* B) {
    return (int)A->id->getValue() - (int)B->id->getValue();
}

DeviceManager::DeviceManager() :
    BaseManager("Device")
    {
    itemDataType = "Device";
    selectItemWhenCreated = true;
    comparator.compareFunc = compare;

}

DeviceManager::~DeviceManager()
{
}


void DeviceManager::addItemInternal(Device* o, var data)
{
    reorderItems();
}

void DeviceManager::removeItemInternal(Device* o)
{
    // o->removeObjectListener(this);
}


