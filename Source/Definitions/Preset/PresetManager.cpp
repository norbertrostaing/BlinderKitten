#include "Preset.h"
#include "PresetManager.h"

juce_ImplementSingleton(PresetManager);

int compare(Preset* A, Preset* B) {
    return (int)A->id->getValue() - (int)B->id->getValue();
}


PresetManager::PresetManager() :
    BaseManager("Preset")
    {
    itemDataType = "Preset";
    selectItemWhenCreated = true;
    comparator.compareFunc = compare;

}

PresetManager::~PresetManager()
{
}


void PresetManager::addItemInternal(Preset* o, var data)
{
    reorderItems();
}

void PresetManager::removeItemInternal(Preset* o)
{
    // o->removeObjectListener(this);
}


