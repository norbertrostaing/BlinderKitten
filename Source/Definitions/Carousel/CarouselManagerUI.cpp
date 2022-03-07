/*
  ==============================================================================

    ChannelFamilyManagerUI.cpp
    Created: 4 Nov 2021 12:32:12am
    Author:  No

  ==============================================================================
*/

#include "CarouselManagerUI.h"
#include "CarouselManager.h"



CarouselManagerUI::CarouselManagerUI(const String & contentName) :
	BaseManagerShapeShifterUI(contentName, CarouselManager::getInstance())
{
	addItemText = "Add new Carousel";
	noItemText = "Carousels are a mix between sequence and effect !";
	// setShowAddButton(false);
	// setShowSearchBar(false);
	addExistingItems();
}

CarouselManagerUI::~CarouselManagerUI()
{
}
