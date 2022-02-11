/*
  ==============================================================================

    PresetTypeChannel.cpp
    Created: 8 Nov 2021 7:28:28pm
    Author:  No

  ==============================================================================
*/

#include "PresetSubFixtureValues.h"
#include "PresetValue.h"

PresetSubFixtureValues::PresetSubFixtureValues(var params) :
    BaseItem(params.getProperty("name", "SubFixture Value")),
    objectType(params.getProperty("type", "PresetSubFixtureValues").toString()),
    objectData(params),
    values()
{
    targetFixtureId = addIntParameter("Fixture ID", "ID of the target Fixture (0 means disabled, used only if preset is in same parameters mode", 0, 0);
    targetSubFixtureId = addIntParameter("SubFixture ID", "ID of the target SubFixture (0 means disabled, used only if preset is in same parameters mode", 0, 0);

    BaseManager<PresetValue>* mv = new BaseManager<PresetValue>("Values");
    mv->selectItemWhenCreated = false;
    values.reset(mv);
    addChildControllableContainer(values.get());
    saveAndLoadRecursiveData = true;
};

PresetSubFixtureValues::~PresetSubFixtureValues()
{
};


