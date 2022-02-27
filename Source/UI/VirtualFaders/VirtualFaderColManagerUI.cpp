/*
  ==============================================================================

    ChannelFamilyManagerUI.cpp
    Created: 4 Nov 2021 12:32:12am
    Author:  No

  ==============================================================================
*/

#include "VirtualFaderColManagerUI.h"
#include "VirtualFaderColManager.h"



VirtualFaderColManagerUI::VirtualFaderColManagerUI(const String & contentName) :
	BaseManagerShapeShifterUI(contentName, VirtualFaderColManager::getInstance())
{
	addItemText = "Add new VirtualFaderCol";
	noItemText = "VirtualFaderCol are re-usable selections of SubFixtures.";
	// setShowAddButton(false);
	// setShowSearchBar(false);
	addExistingItems();
}

VirtualFaderColManagerUI::~VirtualFaderColManagerUI()
{
}
