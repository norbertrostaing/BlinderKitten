/*
  ==============================================================================

    ChannelFamilyManagerUI.cpp
    Created: 4 Nov 2021 12:32:12am
    Author:  No

  ==============================================================================
*/

#include "CurvePresetManagerUI.h"
#include "CurvePresetManager.h"



CurvePresetManagerUI::CurvePresetManagerUI(const String & contentName) :
	BaseManagerShapeShifterUI(contentName, CurvePresetManager::getInstance())
{
	addItemText = "Add new CurvePreset";
	noItemText = "Draw here curves you wanna use often";
	// setShowAddButton(false);
	// setShowSearchBar(false);
	addExistingItems();
}

CurvePresetManagerUI::~CurvePresetManagerUI()
{
}
