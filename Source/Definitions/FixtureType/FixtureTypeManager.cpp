#include "FixtureType.h"
#include "FixtureTypeManager.h"

/*
  ==============================================================================

    ObjectManager.cpp
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

juce_ImplementSingleton(FixtureTypeManager);


FixtureTypeManager::FixtureTypeManager() :
    BaseManager("FixtureType")
    {
    itemDataType = "FixtureType";
    selectItemWhenCreated = true;
       
}

FixtureTypeManager::~FixtureTypeManager()
{
    // stopThread(1000);
}


void FixtureTypeManager::addItemInternal(FixtureType* o, var data)
{
    // o->addChannelFamilyListener(this);
    // if (!isCurrentlyLoadingData) o->globalID->setValue(getFirstAvailableObjectID(o));
}

void FixtureTypeManager::removeItemInternal(FixtureType* o)
{
    // o->removeObjectListener(this);
}


void FixtureTypeManager::onContainerParameterChanged(Parameter* p)
{
   // if (p == lockUI) for (auto& i : items) i->isUILocked->setValue(lockUI->boolValue());
}

