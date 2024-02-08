/*
  ==============================================================================

    ChannelFamilyManagerUI.cpp
    Created: 4 Nov 2021 12:32:12am
    Author:  No

  ==============================================================================
*/

#include "TrackerManagerUI.h"
#include "TrackerManager.h"



TrackerManagerUI::TrackerManagerUI(const String & contentName) :
	BaseManagerShapeShifterUI(contentName, TrackerManager::getInstance())
{
	addItemText = "Add new Tracker";
	noItemText = "Trackers are a mix between sequence and effect !";
	// setShowAddButton(false);
	// setShowSearchBar(false);
	addExistingItems();
}

TrackerManagerUI::~TrackerManagerUI()
{
}
