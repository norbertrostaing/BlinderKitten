/*
  ==============================================================================

    FixtureParamTypeManagerUI.cpp
    Created: 4 Nov 2021 12:32:12am
    Author:  No

  ==============================================================================
*/

#include "FixtureParamTypeManagerUI.h"
#include "FixtureParamTypeManager.h"



FixtureParamTypeManagerUI::FixtureParamTypeManagerUI(const String & contentName) :
	BaseManagerShapeShifterUI(contentName, FixtureParamTypeManager::getInstance())
{
	addItemText = "Add new channel Family";
	noItemText = "Create here families of channels and channels you wanna use in the software.";
	// setShowAddButton(false);
	setShowSearchBar(false);
	addExistingItems();
}

FixtureParamTypeManagerUI::~FixtureParamTypeManagerUI()
{
}
