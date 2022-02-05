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
	// addItemText = "Add Custom Variable Group";
	// noItemText = "You can manage you own variables by first creating a group here.";
	// setShowAddButton(false);
	setShowSearchBar(false);
	addExistingItems();
}

FixtureParamTypeManagerUI::~FixtureParamTypeManagerUI()
{
}
