/*
  ==============================================================================

    FixtureParamTypeManagerUI.cpp
    Created: 4 Nov 2021 12:32:12am
    Author:  No

  ==============================================================================
*/

#include "CuelistManagerUI.h"
#include "CuelistManager.h"



CuelistManagerUI::CuelistManagerUI(const String & contentName) :
	BaseManagerShapeShifterUI(contentName, CuelistManager::getInstance())
{
	addItemText = "Add new Cuelist";
	noItemText = "Add cuelists here";
	// setShowAddButton(false);
	// setShowSearchBar(false);
	addExistingItems();
}

CuelistManagerUI::~CuelistManagerUI()
{
}
