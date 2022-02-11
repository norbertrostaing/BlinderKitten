/*
  ==============================================================================

    ChannelFamilyManagerUI.cpp
    Created: 4 Nov 2021 12:32:12am
    Author:  No

  ==============================================================================
*/

#include "FixtureTypeManagerUI.h"
#include "FixtureTypeManager.h"



FixtureTypeManagerUI::FixtureTypeManagerUI(const String & contentName) :
	BaseManagerShapeShifterUI(contentName, FixtureTypeManager::getInstance())
{
	addItemText = "Add new Fixture type";
	noItemText = "Set here what kind of Fixtures you wanna use.";
	// setShowAddButton(false);
	// setShowSearchBar(false);
	addExistingItems();
}

FixtureTypeManagerUI::~FixtureTypeManagerUI()
{
}
