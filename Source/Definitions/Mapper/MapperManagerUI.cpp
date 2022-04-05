/*
  ==============================================================================

    ChannelFamilyManagerUI.cpp
    Created: 4 Nov 2021 12:32:12am
    Author:  No

  ==============================================================================
*/

#include "MapperManagerUI.h"
#include "MapperManager.h"



MapperManagerUI::MapperManagerUI(const String & contentName) :
	BaseManagerShapeShifterUI(contentName, MapperManager::getInstance())
{
	addItemText = "Add new Mapper";
	noItemText = "Mappers are a mix between sequence and effect !";
	// setShowAddButton(false);
	// setShowSearchBar(false);
	addExistingItems();
}

MapperManagerUI::~MapperManagerUI()
{
}
