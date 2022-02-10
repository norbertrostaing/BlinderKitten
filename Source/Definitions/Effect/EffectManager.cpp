#include "Effect.h"
#include "EffectManager.h"

/*
  ==============================================================================

    ObjectManager.cpp
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

juce_ImplementSingleton(EffectManager);

int compare(Effect* A, Effect* B) {
    return (int)A->id->getValue() - (int)B->id->getValue();
}

EffectManager::EffectManager() :
    BaseManager("Effect")
    {
    itemDataType = "Effect";
    selectItemWhenCreated = true;
    comparator.compareFunc = compare;
}

EffectManager::~EffectManager()
{
    // stopThread(1000);
}


void EffectManager::addItemInternal(Effect* o, var data)
{
    reorderItems();
}

void EffectManager::removeItemInternal(Effect* o)
{
    // o->removeObjectListener(this);
}


void EffectManager::onContainerParameterChanged(Parameter* p)
{
   // if (p == lockUI) for (auto& i : items) i->isUILocked->setValue(lockUI->boolValue());
}

