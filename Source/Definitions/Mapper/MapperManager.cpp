#include "Mapper.h"
#include "MapperManager.h"

/*
  ==============================================================================

    ObjectManager.cpp
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

juce_ImplementSingleton(MapperManager);

int compare(Mapper* A, Mapper* B) {
    return (int)A->id->getValue() - (int)B->id->getValue();
}

MapperManager::MapperManager() :
    BaseManager("Mapper")
    {
    itemDataType = "Mapper";
    selectItemWhenCreated = true;
    comparator.compareFunc = compare;
}

MapperManager::~MapperManager()
{
    // stopThread(1000);
}


void MapperManager::addItemInternal(Mapper* o, var data)
{
    reorderItems();
}

void MapperManager::removeItemInternal(Mapper* o)
{
    // o->removeObjectListener(this);
}


void MapperManager::onContainerParameterChanged(Parameter* p)
{
   // if (p == lockUI) for (auto& i : items) i->isUILocked->setValue(lockUI->boolValue());
}

