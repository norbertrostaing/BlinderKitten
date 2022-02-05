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
	addItemText = "Add new fixture type";
	noItemText = "You must create fixture types to patch new fixtures";
	// setShowAddButton(false);
	// setShowSearchBar(false);
	addExistingItems();
}

DeviceTypeManagerUI::~DeviceTypeManagerUI()
{
}
