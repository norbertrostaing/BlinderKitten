/*
  ==============================================================================

    ChannelFamilyManagerUI.cpp
    Created: 4 Nov 2021 12:32:12am
    Author:  No

  ==============================================================================
*/

#include "PresetManagerUI.h"
#include "PresetManager.h"



PresetManagerUI::PresetManagerUI(const String & contentName) :
	BaseManagerShapeShifterUI(contentName, PresetManager::getInstance())
{
	addItemText = "Add new preset";
	noItemText = "Create here your beautiful presets.";
	// setShowAddButton(false);
	// setShowSearchBar(false);
	addExistingItems();
	//paintOverChildren
	//paintEntireComponent
	//paint
}

PresetManagerUI::~PresetManagerUI()
{
}
