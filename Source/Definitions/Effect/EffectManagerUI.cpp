/*
  ==============================================================================

    FixtureParamTypeManagerUI.cpp
    Created: 4 Nov 2021 12:32:12am
    Author:  No

  ==============================================================================
*/

#include "EffectManagerUI.h"
#include "EffectManager.h"



EffectManagerUI::EffectManagerUI(const String & contentName) :
	BaseManagerShapeShifterUI(contentName, EffectManager::getInstance())
{
	addItemText = "Add new Effect";
	noItemText = "Welcome in Effects land :)";
	// setShowAddButton(false);
	// setShowSearchBar(false);
	addExistingItems();
}

EffectManagerUI::~EffectManagerUI()
{
}
