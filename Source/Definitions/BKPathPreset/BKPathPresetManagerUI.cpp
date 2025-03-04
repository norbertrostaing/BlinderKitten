/*
  ==============================================================================

    ChannelFamilyManagerUI.cpp
    Created: 4 Nov 2021 12:32:12am
    Author:  No

  ==============================================================================
*/

#include "BKPathPresetManagerUI.h"
#include "BKPathPresetManager.h"



BKPathPresetManagerUI::BKPathPresetManagerUI(const String & contentName) :
	BaseManagerShapeShifterUI(contentName, BKPathPresetManager::getInstance())
{
	addItemText = "Add new BKPathPreset";
	noItemText = "Store here your beautiful delays and fade combination";
	// setShowAddButton(false);
	// setShowSearchBar(false);
	addExistingItems();
}

BKPathPresetManagerUI::~BKPathPresetManagerUI()
{
}
