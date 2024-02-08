#include "Tracker.h"
#include "TrackerManager.h"

/*
  ==============================================================================

    ObjectManager.cpp
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

juce_ImplementSingleton(TrackerManager);

int compare(Tracker* A, Tracker* B) {
    return (int)A->id->getValue() - (int)B->id->getValue();
}

TrackerManager::TrackerManager() :
    BaseManager("Tracker")
    {
    itemDataType = "Tracker";
    selectItemWhenCreated = true;
    comparator.compareFunc = compare;
}

TrackerManager::~TrackerManager()
{
    // stopThread(1000);
}


void TrackerManager::addItemInternal(Tracker* o, var data)
{
    reorderItems();
}

void TrackerManager::removeItemInternal(Tracker* o)
{
    // o->removeObjectListener(this);
}


void TrackerManager::onContainerParameterChanged(Parameter* p)
{
   // if (p == lockUI) for (auto& i : items) i->isUILocked->setValue(lockUI->boolValue());
}

void TrackerManager::recomputeAllTrackers()
{
    for (int i = 0; i < items.size(); i++) {
        items[i]->pleaseComputeIfRunning();
    }
}

