/*
  ==============================================================================

    ChannelFamilyManagerUI.cpp
    Created: 4 Nov 2021 12:32:12am
    Author:  No

  ==============================================================================
*/

#include "VirtualButtonManagerUI.h"
#include "VirtualButtonManager.h"



VirtualButtonManagerUI::VirtualButtonManagerUI(const String & contentName) :
	BaseManagerShapeShifterUI(contentName, VirtualButtonManager::getInstance())
{
	addItemText = "Add new VirtualButton";
	noItemText = "VirtualButton are re-usable selections of SubFixtures.";
	// setShowAddButton(false);
	// setShowSearchBar(false);
	addExistingItems();
}

VirtualButtonManagerUI::~VirtualButtonManagerUI()
{
}
