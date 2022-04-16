#include "Multiplicator.h"
#include "MultiplicatorManager.h"
/*
  ==============================================================================

    ObjectManager.cpp
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

juce_ImplementSingleton(MultiplicatorManager);


MultiplicatorManager::MultiplicatorManager() :
    BaseManager("Multiplicators")    {
    itemDataType = "Multiplicator";
    selectItemWhenCreated = true;
       
}

MultiplicatorManager::~MultiplicatorManager()
{
}


void MultiplicatorManager::addItemInternal(Multiplicator* o, var data)
{
    // o->addMultiplicatorListener(this);
    // if (!isCurrentlyLoadingData) o->globalID->setValue(getFirstAvailableObjectID(o));
}

void MultiplicatorManager::removeItemInternal(Multiplicator* o)
{
    // o->removeObjectListener(this);
}


void MultiplicatorManager::onContainerParameterChanged(Parameter* p)
{
   // if (p == lockUI) for (auto& i : items) i->isUILocked->setValue(lockUI->boolValue());
}
