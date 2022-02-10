/*
  ==============================================================================

    FixtureParamTypeManagerUI.cpp
    Created: 4 Nov 2021 12:32:12am
    Author:  No

  ==============================================================================
*/

#include "DeviceTypeManagerUI.h"
#include "DeviceTypeManager.h"



DeviceTypeManagerUI::DeviceTypeManagerUI(const String & contentName) :
	BaseManagerShapeShifterUI(contentName, DeviceTypeManager::getInstance())
{
	addItemText = "Add new device type";
	noItemText = "Set here what kind of devices you wanna use.";
	// setShowAddButton(false);
	// setShowSearchBar(false);
	addExistingItems();
}

DeviceTypeManagerUI::~DeviceTypeManagerUI()
{
}
