#include "Stamp.h"
#include "StampManager.h"

/*
  ==============================================================================

    ObjectManager.cpp
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

juce_ImplementSingleton(StampManager);

int compare(Stamp* A, Stamp* B) {
    return (int)A->id->getValue() - (int)B->id->getValue();
}

StampManager::StampManager() :
    BaseManager("Stamp")
    {
    itemDataType = "Stamp";
    selectItemWhenCreated = true;
    comparator.compareFunc = compare;
}

StampManager::~StampManager()
{
    // stopThread(1000);
}


void StampManager::addItemInternal(Stamp* o, var data)
{
    reorderItems();
}

void StampManager::removeItemInternal(Stamp* o)
{
    // o->removeObjectListener(this);
}


void StampManager::onContainerParameterChanged(Parameter* p)
{
   // if (p == lockUI) for (auto& i : items) i->isUILocked->setValue(lockUI->boolValue());
}

