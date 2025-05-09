#include "SelectionMaster.h"
#include "SelectionMasterManager.h"

/*
  ==============================================================================

    ObjectManager.cpp
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

juce_ImplementSingleton(SelectionMasterManager);

int compare(SelectionMaster* A, SelectionMaster* B) {
    return (int)A->id->getValue() - (int)B->id->getValue();
}

SelectionMasterManager::SelectionMasterManager() :
    BaseManager("SelectionMaster")
    {
    itemDataType = "SelectionMaster";
    selectItemWhenCreated = true;
    comparator.compareFunc = compare;
}

SelectionMasterManager::~SelectionMasterManager()
{
    // stopThread(1000);
}


void SelectionMasterManager::addItemInternal(SelectionMaster* o, var data)
{
    reorderItems();
}

void SelectionMasterManager::removeItemInternal(SelectionMaster* o)
{
    // o->removeObjectListener(this);
}


void SelectionMasterManager::onContainerParameterChanged(Parameter* p)
{
   // if (p == lockUI) for (auto& i : items) i->isUILocked->setValue(lockUI->boolValue());
}

void SelectionMasterManager::recomputeAllSelectionMasters()
{
    for (int i = 0; i < items.size(); i++) {
        //items[i]->pleaseComputeIfRunning();
    }
}

