#include "Device.h"
#include "DeviceManager.h"

juce_ImplementSingleton(DeviceManager);


DeviceManager::DeviceManager() :
    BaseManager("Device")
    {
    itemDataType = "Device";
    selectItemWhenCreated = true;
       
}

DeviceManager::~DeviceManager()
{
}


void DeviceManager::addItemInternal(Device* o, var data)
{
    // o->addFixtureParamTypeListener(this);
    // if (!isCurrentlyLoadingData) o->globalID->setValue(getFirstAvailableObjectID(o));
}

void DeviceManager::removeItemInternal(Device* o)
{
    // o->removeObjectListener(this);
}


