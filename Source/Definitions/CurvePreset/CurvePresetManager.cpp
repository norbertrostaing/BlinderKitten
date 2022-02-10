#include "CurvePreset.h"
#include "CurvePresetManager.h"

/*
  ==============================================================================

    ObjectManager.cpp
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

juce_ImplementSingleton(CurvePresetManager);

int compare(CurvePreset* A, CurvePreset* B) {
    return (int)A->id->getValue() - (int)B->id->getValue();
}

CurvePresetManager::CurvePresetManager() :
    BaseManager("CurvePreset")
    {
    itemDataType = "CurvePreset";
    selectItemWhenCreated = true;
    comparator.compareFunc = compare;
}

CurvePresetManager::~CurvePresetManager()
{
    // stopThread(1000);
}


void CurvePresetManager::addItemInternal(CurvePreset* o, var data)
{
    reorderItems();
}

void CurvePresetManager::removeItemInternal(CurvePreset* o)
{
    // o->removeObjectListener(this);
}


void CurvePresetManager::onContainerParameterChanged(Parameter* p)
{
   // if (p == lockUI) for (auto& i : items) i->isUILocked->setValue(lockUI->boolValue());
}

