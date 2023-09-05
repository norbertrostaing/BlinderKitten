#include "Layout.h"
#include "LayoutManager.h"
/*
  ==============================================================================

    ObjectManager.cpp
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

juce_ImplementSingleton(LayoutManager);

int compare(Layout* A, Layout* B) {
    return (int)A->id->getValue() - (int)B->id->getValue();
}



LayoutManager::LayoutManager() :
    BaseManager("2D Plans")    {
    itemDataType = "Layout";
    selectItemWhenCreated = true;
    comparator.compareFunc = compare;

}

LayoutManager::~LayoutManager()
{
}


void LayoutManager::addItemInternal(Layout* o, var data)
{
    reorderItems();
}

void LayoutManager::removeItemInternal(Layout* o)
{
    // o->removeObjectListener(this);
}


void LayoutManager::onContainerParameterChanged(Parameter* p)
{
   // if (p == lockUI) for (auto& i : items) i->isUILocked->setValue(lockUI->boolValue());
}

