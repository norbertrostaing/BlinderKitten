/*
  ==============================================================================

    ChannelFamilyManagerUI.cpp
    Created: 4 Nov 2021 12:32:12am
    Author:  No

  ==============================================================================
*/

#include "BundleManagerUI.h"
#include "BundleManager.h"

BundleManagerUI::BundleManagerUI(const String & contentName) :
	BaseManagerShapeShifterUI(contentName, BundleManager::getInstance())
{
	addItemText = "Add new Bundle (temp)";
	noItemText = "You must create SubFixture types to patch new SubFixtures";
	// setShowAddButton(false);
	// setShowSearchBar(false);
	addExistingItems();
}

BundleManagerUI::~BundleManagerUI()
{
}
