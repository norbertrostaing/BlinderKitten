/*
  ==============================================================================

    ChannelFamilyManagerUI.cpp
    Created: 4 Nov 2021 12:32:12am
    Author:  No

  ==============================================================================
*/

#include "SelectionMasterManagerUI.h"
#include "SelectionMasterManager.h"



SelectionMasterManagerUI::SelectionMasterManagerUI(const String & contentName) :
	BaseManagerShapeShifterUI(contentName, SelectionMasterManager::getInstance())
{
	addItemText = "Add new SelectionMaster";
	noItemText = "SelectionMasters are a mix between sequence and effect !";
	// setShowAddButton(false);
	// setShowSearchBar(false);
	addExistingItems();
}

SelectionMasterManagerUI::~SelectionMasterManagerUI()
{
}
