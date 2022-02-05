/*
  ==============================================================================

    FixtureParamTypeManagerUI.cpp
    Created: 4 Nov 2021 12:32:12am
    Author:  No

  ==============================================================================
*/

#include "CommandManagerUI.h"
#include "CommandManager.h"

CommandManagerUI::CommandManagerUI(const String & contentName) :
	BaseManagerShapeShifterUI(contentName, CommandManager::getInstance())
{
	addItemText = "Add new command (temp)";
	noItemText = "You must create fixture types to patch new fixtures";
	// setShowAddButton(false);
	// setShowSearchBar(false);
	addExistingItems();
}

CommandManagerUI::~CommandManagerUI()
{
}
