#include "BKPathPreset.h"
#include "BKPathPresetManager.h"

/*
  ==============================================================================

    ObjectManager.cpp
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

juce_ImplementSingleton(BKPathPresetManager);

int compare(BKPathPreset* A, BKPathPreset* B) {
    return (int)A->id->getValue() - (int)B->id->getValue();
}

BKPathPresetManager::BKPathPresetManager() :
    BaseManager("BKPathPreset")
    {
    itemDataType = "BKPathPreset";
    selectItemWhenCreated = true;
    comparator.compareFunc = compare;
}

BKPathPresetManager::~BKPathPresetManager()
{
    // stopThread(1000);
}


void BKPathPresetManager::addItemInternal(BKPathPreset* o, var data)
{
    reorderItems();
}

void BKPathPresetManager::removeItemInternal(BKPathPreset* o)
{
    // o->removeObjectListener(this);
}


void BKPathPresetManager::onContainerParameterChanged(Parameter* p)
{
   // if (p == lockUI) for (auto& i : items) i->isUILocked->setValue(lockUI->boolValue());
}

