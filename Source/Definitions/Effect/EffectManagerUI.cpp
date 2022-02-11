/*
  ==============================================================================

    ChannelFamilyManagerUI.cpp
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
	noItemText = "Effects allows you add a curve to a value to change it over time";
	// setShowAddButton(false);
	// setShowSearchBar(false);
	addExistingItems();
}

EffectManagerUI::~EffectManagerUI()
{
}
