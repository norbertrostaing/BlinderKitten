#include "DeviceType.h"
#include "DeviceTypeManager.h"

/*
  ==============================================================================

    ObjectManager.cpp
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

juce_ImplementSingleton(DeviceTypeManager);


DeviceTypeManager::DeviceTypeManager() :
    BaseManager("DeviceType")
    {
    itemDataType = "DeviceType";
    selectItemWhenCreated = true;
       
}

DeviceTypeManager::~DeviceTypeManager()
{
    // stopThread(1000);
}


void DeviceTypeManager::addItemInternal(DeviceType* o, var data)
{
    // o->addFixtureParamTypeListener(this);
    // if (!isCurrentlyLoadingData) o->globalID->setValue(getFirstAvailableObjectID(o));
}

void DeviceTypeManager::removeItemInternal(DeviceType* o)
{
    // o->removeObjectListener(this);
}


void DeviceTypeManager::onContainerParameterChanged(Parameter* p)
{
   // if (p == lockUI) for (auto& i : items) i->isUILocked->setValue(lockUI->boolValue());
}

