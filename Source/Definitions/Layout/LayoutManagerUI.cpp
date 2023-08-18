/*
  ==============================================================================

    LayoutManagerUI.cpp
    Created: 4 Nov 2021 12:32:12am
    Author:  No

  ==============================================================================
*/

#include "LayoutManagerUI.h"
#include "LayoutManager.h"



LayoutManagerUI::LayoutManagerUI(const String & contentName) :
	BaseManagerShapeShifterUI(contentName, LayoutManager::getInstance())
{
	addItemText = "Add new layout";
	noItemText = "Each layout is a 2D View of your groups and fixtures.";
	// setShowAddButton(false);
	setShowSearchBar(false);
	addExistingItems();
}

LayoutManagerUI::~LayoutManagerUI()
{
}
