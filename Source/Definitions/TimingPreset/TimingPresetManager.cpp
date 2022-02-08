#include "TimingPreset.h"
#include "TimingPresetManager.h"

/*
  ==============================================================================

    ObjectManager.cpp
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

juce_ImplementSingleton(TimingPresetManager);


TimingPresetManager::TimingPresetManager() :
    BaseManager("TimingPreset")
    {
    itemDataType = "TimingPreset";
    selectItemWhenCreated = true;
}

TimingPresetManager::~TimingPresetManager()
{
    // stopThread(1000);
}


void TimingPresetManager::addItemInternal(TimingPreset* o, var data)
{
    // o->addFixtureParamTypeListener(this);
    // if (!isCurrentlyLoadingData) o->globalID->setValue(getFirstAvailableObjectID(o));
}

void TimingPresetManager::removeItemInternal(TimingPreset* o)
{
    // o->removeObjectListener(this);
}


void TimingPresetManager::onContainerParameterChanged(Parameter* p)
{
   // if (p == lockUI) for (auto& i : items) i->isUILocked->setValue(lockUI->boolValue());
}

