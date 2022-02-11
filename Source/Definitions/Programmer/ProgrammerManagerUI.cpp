/*
  ==============================================================================

    ChannelFamilyManagerUI.cpp
    Created: 4 Nov 2021 12:32:12am
    Author:  No

  ==============================================================================
*/

#include "ProgrammerManagerUI.h"
#include "ProgrammerManager.h"



ProgrammerManagerUI::ProgrammerManagerUI(const String & contentName) :
	BaseManagerShapeShifterUI(contentName, ProgrammerManager::getInstance())
{
	addItemText = "Add new programmer";
	noItemText = "Programmers allow yo to have a direct control to data, and you can record their content in various targets (groups, presets, cuelists...)";
	// setShowAddButton(false);
	// setShowSearchBar(false);
	addExistingItems();
}

ProgrammerManagerUI::~ProgrammerManagerUI()
{
}
