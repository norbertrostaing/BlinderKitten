/*
  ==============================================================================

    ChannelFamilyManagerUI.cpp
    Created: 4 Nov 2021 12:32:12am
    Author:  No

  ==============================================================================
*/

#include "MediaManagerUI.h"
#include "MediaManager.h"



MediaManagerUI::MediaManagerUI(const String & contentName) :
	BaseManagerShapeShifterUI(contentName, MediaManager::getInstance())
{
	addItemText = "Add new Media";
	noItemText = "Medias are re-usable selections of SubFixtures.";
	// setShowAddButton(false);
	// setShowSearchBar(false);
	addExistingItems();
}

MediaManagerUI::~MediaManagerUI()
{
}

