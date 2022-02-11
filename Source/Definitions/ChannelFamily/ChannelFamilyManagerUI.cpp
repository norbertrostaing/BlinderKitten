/*
  ==============================================================================

    ChannelFamilyManagerUI.cpp
    Created: 4 Nov 2021 12:32:12am
    Author:  No

  ==============================================================================
*/

#include "ChannelFamilyManagerUI.h"
#include "ChannelFamilyManager.h"



ChannelFamilyManagerUI::ChannelFamilyManagerUI(const String & contentName) :
	BaseManagerShapeShifterUI(contentName, ChannelFamilyManager::getInstance())
{
	addItemText = "Add new channel Family";
	noItemText = "Create here families of channels and channels you wanna use in the software.";
	// setShowAddButton(false);
	setShowSearchBar(false);
	addExistingItems();
}

ChannelFamilyManagerUI::~ChannelFamilyManagerUI()
{
}
