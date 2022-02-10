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

int compare(Programmer* A, Programmer* B) {
    return (int)A->id->getValue() - (int)B->id->getValue();
}


ProgrammerManager::ProgrammerManager() :
    BaseManager("Programmer")
    {
    itemDataType = "Programmer";
    selectItemWhenCreated = true;
    comparator.compareFunc = compare;

}

ProgrammerManager::~ProgrammerManager()
{
    // stopThread(1000);
}


void ProgrammerManager::addItemInternal(Programmer* o, var data)
{
    reorderItems();
}

void ProgrammerManager::removeItemInternal(Programmer* o)
{
    // o->removeObjectListener(this);
}


void ProgrammerManager::onContainerParameterChanged(Parameter* p)
{
   // if (p == lockUI) for (auto& i : items) i->isUILocked->setValue(lockUI->boolValue());
}

