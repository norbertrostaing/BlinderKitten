/*
  ==============================================================================

    PresetTypeChannel.cpp
    Created: 8 Nov 2021 7:28:28pm
    Author:  No

  ==============================================================================
*/

#include "PresetSubFixtureValues.h"
#include "PresetValue.h"
#include "Definitions/ChannelFamily/ChannelFamilyManager.h"

int comparePresetValues(PresetValue* A, PresetValue* B) {
    ChannelType* typeA = dynamic_cast<ChannelType*>(A->param->targetContainer.get());
    ChannelType* typeB = dynamic_cast<ChannelType*>(B->param->targetContainer.get());

    int indexA = ChannelFamilyManager::getInstance()->orderedElements.indexOf(typeA);
    int indexB = ChannelFamilyManager::getInstance()->orderedElements.indexOf(typeB);

    return indexA-indexB;
}



PresetSubFixtureValues::PresetSubFixtureValues(var params) :
    BaseItem(params.getProperty("name", "SubFixture Value")),
    objectType(params.getProperty("type", "PresetSubFixtureValues").toString()),
    objectData(params),
    values("Values")
{
    targetFixtureId = addIntParameter("Fixture ID", "ID of the target Fixture (0 means disabled, used only if preset is in same parameters mode", 0, 0);
    targetSubFixtureId = addIntParameter("SubFixture ID", "ID of the target SubFixture (0 means disabled, used only if preset is in same parameters mode", 0, 0);

    values.selectItemWhenCreated = false;
    values.comparator.compareFunc = comparePresetValues;
    addChildControllableContainer(&values);
    saveAndLoadRecursiveData = true;
    if (params.isVoid()) {
        values.addItem();
    }
};

PresetSubFixtureValues::~PresetSubFixtureValues()
{
};


