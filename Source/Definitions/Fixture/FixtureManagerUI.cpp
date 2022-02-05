/*
  ==============================================================================

    FixtureParamTypeManagerUI.cpp
    Created: 4 Nov 2021 12:32:12am
    Author:  No

  ==============================================================================
*/

#include "FixtureManagerUI.h"
#include "FixtureManager.h"



FixtureManagerUI::FixtureManagerUI(const String & contentName) :
	BaseManagerShapeShifterUI(contentName, FixtureManager::getInstance())
{
	// addItemText = "Add new fixture type";
	noItemText = "You must patch some devices first :)";
	// setShowAddButton(false);
	// setShowSearchBar(false);
	addExistingItems();
}

FixtureManagerUI::~FixtureManagerUI()
{
}
