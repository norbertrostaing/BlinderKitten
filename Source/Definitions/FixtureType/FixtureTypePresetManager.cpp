#include "FixtureTypePresetManager.h"
#include "../../Brain.h"

FixtureTypePresetManager::FixtureTypePresetManager() :
    BaseManager("Presets")
{
    itemDataType = "FixtureTypePreset";
    selectItemWhenCreated = false;       
}

FixtureTypePresetManager::~FixtureTypePresetManager()
{
}

void FixtureTypePresetManager::addItemInternal(FixtureTypePreset* c, var data)
{
}
