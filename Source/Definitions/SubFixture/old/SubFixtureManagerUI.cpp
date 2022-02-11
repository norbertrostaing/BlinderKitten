/*
  ==============================================================================

    ChannelFamilyManagerUI.cpp
    Created: 4 Nov 2021 12:32:12am
    Author:  No

  ==============================================================================
*/

#include "SubFixtureManagerUI.h"
#include "SubFixtureManager.h"



SubFixtureManagerUI::SubFixtureManagerUI(const String & contentName) :
	BaseManagerShapeShifterUI(contentName, SubFixtureManager::getInstance())
{
	// addItemText = "Add new SubFixture type";
	noItemText = "You must patch some Fixtures first :)";
	// setShowAddButton(false);
	// setShowSearchBar(false);
	addExistingItems();
}

SubFixtureManagerUI::~SubFixtureManagerUI()
{
}
