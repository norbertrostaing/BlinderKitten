/*
  ==============================================================================

    ChannelFamilyManagerUI.cpp
    Created: 4 Nov 2021 12:32:12am
    Author:  No

  ==============================================================================
*/

#include "StampManagerUI.h"
#include "StampManager.h"



StampManagerUI::StampManagerUI(const String & contentName) :
	BaseManagerShapeShifterUI(contentName, StampManager::getInstance())
{
	addItemText = "Add new Stamp";
	noItemText = "Stamps are re-usable selections of SubFixtures.";
	// setShowAddButton(false);
	// setShowSearchBar(false);
	addExistingItems();
}

StampManagerUI::~StampManagerUI()
{
}
