#include "Preset.h"
#include "PresetManager.h"

juce_ImplementSingleton(PresetManager);


PresetManager::PresetManager() :
    BaseManager("Preset")
    {
    itemDataType = "Preset";
    selectItemWhenCreated = true;
       
}

PresetManager::~PresetManager()
{
}


void PresetManager::addItemInternal(Preset* o, var data)
{
    // o->addFixtureParamTypeListener(this);
    // if (!isCurrentlyLoadingData) o->globalID->setValue(getFirstAvailableObjectID(o));
}

void PresetManager::removeItemInternal(Preset* o)
{
    // o->removeObjectListener(this);
}


