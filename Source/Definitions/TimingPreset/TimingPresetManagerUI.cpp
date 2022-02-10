/*
  ==============================================================================

    FixtureParamTypeManagerUI.cpp
    Created: 4 Nov 2021 12:32:12am
    Author:  No

  ==============================================================================
*/

#include "TimingPresetManagerUI.h"
#include "TimingPresetManager.h"



TimingPresetManagerUI::TimingPresetManagerUI(const String & contentName) :
	BaseManagerShapeShifterUI(contentName, TimingPresetManager::getInstance())
{
	addItemText = "Add new TimingPreset";
	noItemText = "Store here your beautiful delays and fade combination";
	// setShowAddButton(false);
	// setShowSearchBar(false);
	addExistingItems();
}

TimingPresetManagerUI::~TimingPresetManagerUI()
{
}
