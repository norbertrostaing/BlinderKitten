/*
  ==============================================================================

    ChannelFamilyManagerUI.cpp
    Created: 4 Nov 2021 12:32:12am
    Author:  No

  ==============================================================================
*/

#include "GroupManagerUI.h"
#include "GroupManager.h"



GroupManagerUI::GroupManagerUI(const String & contentName) :
	BaseManagerShapeShifterUI(contentName, GroupManager::getInstance())
{
	addItemText = "Add new Group";
	noItemText = "Groups are re-usable selections of SubFixtures.";
	// setShowAddButton(false);
	// setShowSearchBar(false);
	addExistingItems();
}

GroupManagerUI::~GroupManagerUI()
{
}
