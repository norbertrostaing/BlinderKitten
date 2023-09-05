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

int compare(Cuelist* A, Cuelist* B) {
    return (int)A->id->getValue() - (int)B->id->getValue();
}

CuelistManager::CuelistManager() :
    BaseManager("Cuelist")
    {
    itemDataType = "Cuelist";
    selectItemWhenCreated = true;
    comparator.compareFunc = compare;
}

CuelistManager::~CuelistManager()
{
    // stopThread(1000);
}


void CuelistManager::addItemInternal(Cuelist* o, var data)
{
    reorderItems();
}

void CuelistManager::removeItemInternal(Cuelist* o)
{
    // o->removeObjectListener(this);
}


void CuelistManager::onContainerParameterChanged(Parameter* p)
{
   // if (p == lockUI) for (auto& i : items) i->isUILocked->setValue(lockUI->boolValue());
}

