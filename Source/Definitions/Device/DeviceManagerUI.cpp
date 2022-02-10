/*
  ==============================================================================

    FixtureParamTypeManagerUI.cpp
    Created: 4 Nov 2021 12:32:12am
    Author:  No

  ==============================================================================
*/

#include "DeviceManagerUI.h"
#include "DeviceManager.h"



DeviceManagerUI::DeviceManagerUI(const String & contentName) :
	BaseManagerShapeShifterUI(contentName, DeviceManager::getInstance())
{
	addItemText = "Add new device";
	noItemText = "It's time to bring your devices type to life, add new devices here, set their type and patch them to begin to play with them";
	// setShowAddButton(false);
	// setShowSearchBar(false);
	addExistingItems();
}

DeviceManagerUI::~DeviceManagerUI()
{
}
