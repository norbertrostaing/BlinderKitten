/*
  ==============================================================================

    MultiplicatorManagerUI.cpp
    Created: 4 Nov 2021 12:32:12am
    Author:  No

  ==============================================================================
*/

#include "MultiplicatorManagerUI.h"
#include "MultiplicatorManager.h"



MultiplicatorManagerUI::MultiplicatorManagerUI(const String & contentName) :
	BaseManagerShapeShifterUI(contentName, MultiplicatorManager::getInstance())
{
	addItemText = "Add new multiplicator";
	noItemText = "Multiplicator can be used in timings, effects, carousels, etc to multiply multiple values at one time";
	// setShowAddButton(false);
	setShowSearchBar(false);
	addExistingItems();
}

MultiplicatorManagerUI::~MultiplicatorManagerUI()
{
}
