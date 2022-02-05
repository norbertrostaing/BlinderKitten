#include "Cuelist.h"
#include "CuelistManager.h"

/*
  ==============================================================================

    ObjectManager.cpp
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

juce_ImplementSingleton(CuelistManager);


CuelistManager::CuelistManager() :
    BaseManager("Cuelist")
    {
    itemDataType = "Cuelist";
    selectItemWhenCreated = true;
       
}

CuelistManager::~CuelistManager()
{
    // stopThread(1000);
}


void CuelistManager::addItemInternal(Cuelist* o, var data)
{
    // o->addFixtureParamTypeListener(this);
    // if (!isCurrentlyLoadingData) o->globalID->setValue(getFirstAvailableObjectID(o));
}

void CuelistManager::removeItemInternal(Cuelist* o)
{
    // o->removeObjectListener(this);
}


void CuelistManager::onContainerParameterChanged(Parameter* p)
{
   // if (p == lockUI) for (auto& i : items) i->isUILocked->setValue(lockUI->boolValue());
}

