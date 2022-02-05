/*
  ==============================================================================

    PresetTypeChannel.cpp
    Created: 8 Nov 2021 7:28:28pm
    Author:  No

  ==============================================================================
*/

#include "PresetFixtureValues.h"
#include "PresetValue.h"

PresetFixtureValues::PresetFixtureValues(var params) :
    BaseItem(params.getProperty("name", "Fixture Value")),
    objectType(params.getProperty("type", "PresetFixtureValues").toString()),
    objectData(params),
    values()
{
    targetFixtureId = addIntParameter("Fixture ID", "ID of the target fixture (0 means disabled, used only if preset is in same parameters mode",0,0);

    BaseManager<PresetValue>* mv = new BaseManager<PresetValue>("Values");
    mv->selectItemWhenCreated = false;
    values.reset(mv);
    addChildControllableContainer(values.get());
    saveAndLoadRecursiveData = true;
};

PresetFixtureValues::~PresetFixtureValues()
{
};


