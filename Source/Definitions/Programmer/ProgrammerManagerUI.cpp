/*
  ==============================================================================

    FixtureParamTypeManagerUI.cpp
    Created: 4 Nov 2021 12:32:12am
    Author:  No

  ==============================================================================
*/

#include "ProgrammerManagerUI.h"
#include "ProgrammerManager.h"



ProgrammerManagerUI::ProgrammerManagerUI(const String & contentName) :
	BaseManagerShapeShifterUI(contentName, ProgrammerManager::getInstance())
{
	addItemText = "Add new cProgrammer";
	noItemText = "Welcome in Programmers land :)";
	// setShowAddButton(false);
	// setShowSearchBar(false);
	addExistingItems();
}

ProgrammerManagerUI::~ProgrammerManagerUI()
{
}
