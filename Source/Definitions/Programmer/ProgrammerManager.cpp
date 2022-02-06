#include "Programmer.h"
#include "ProgrammerManager.h"

/*
  ==============================================================================

    ObjectManager.cpp
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

juce_ImplementSingleton(ProgrammerManager);


ProgrammerManager::ProgrammerManager() :
    BaseManager("Programmer")
    {
    itemDataType = "Programmer";
    selectItemWhenCreated = true;
       
}

ProgrammerManager::~ProgrammerManager()
{
    // stopThread(1000);
}


void ProgrammerManager::addItemInternal(Programmer* o, var data)
{
    // o->addFixtureParamTypeListener(this);
    // if (!isCurrentlyLoadingData) o->globalID->setValue(getFirstAvailableObjectID(o));
}

void ProgrammerManager::removeItemInternal(Programmer* o)
{
    // o->removeObjectListener(this);
}


void ProgrammerManager::onContainerParameterChanged(Parameter* p)
{
   // if (p == lockUI) for (auto& i : items) i->isUILocked->setValue(lockUI->boolValue());
}

