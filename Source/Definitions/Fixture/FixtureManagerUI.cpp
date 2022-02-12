/*
  ==============================================================================

    ChannelFamilyManagerUI.cpp
    Created: 4 Nov 2021 12:32:12am
    Author:  No

  ==============================================================================
*/

#include "FixtureManagerUI.h"
#include "FixtureManager.h"
#include "Fixture.h"

#include "../../MainComponent.h"

FixtureManagerUI::FixtureManagerUI(const String & contentName) :
	BaseManagerShapeShifterUI(contentName, FixtureManager::getInstance())
{
	addItemText = "Add new Fixture";
	noItemText = "It's time to bring your Fixtures type to life, add new Fixtures here, set their type and patch them to begin to play with them";
	// setShowAddButton(false);
	// setShowSearchBar(false);
	addExistingItems();
}

FixtureManagerUI::~FixtureManagerUI()
{
}

