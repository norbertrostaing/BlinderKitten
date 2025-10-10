/*
  ==============================================================================

    FixtureTypePresetValues.cpp
    Created: 8 Nov 2021 7:28:28pm
    Author:  No

  ==============================================================================
*/

#include "FixtureTypePresetValues.h"
#include "../ChannelFamily/ChannelFamilyManager.h"
#include "Fixture/FixtureManager.h"

FixtureTypePresetValues::FixtureTypePresetValues(var params) :
    BaseItem(params.getProperty("name", "Preset Values")),
    objectType(params.getProperty("type", "FixtureTypePresetValues").toString()),
    objectData(params),
    values("Values")
{
    itemDataType = "FixtureTypePresetValues";
    canBeDisabled = true;

    subFixtureId = addIntParameter("Subfixture ID", "id of the subfixture for this preset", 0,0);
    subFixtureId->canBeDisabledByUser = true;
    subFixtureId->setEnabled(false, true);
    addChildControllableContainer(&values);
    values.selectItemWhenCreated = false;
    saveAndLoadRecursiveData = true;
};

FixtureTypePresetValues::~FixtureTypePresetValues()
{
};

void FixtureTypePresetValues::onContainerParameterChangedInternal(Parameter* p) {
}
