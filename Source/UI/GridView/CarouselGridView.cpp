/*
  ==============================================================================

    CarouselGridView.cpp
    Created: 19 Feb 2022 12:19:42am
    Author:  No

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CarouselGridView.h"
#include "Brain.h"
#include "Definitions/Carousel/Carousel.h"
#include "Definitions/Carousel/CarouselManager.h"

//==============================================================================
CarouselGridViewUI::CarouselGridViewUI(const String& contentName):
    ShapeShifterContent(CarouselGridView::getInstance(), contentName)
{
    
}

CarouselGridViewUI::~CarouselGridViewUI()
{
}

juce_ImplementSingleton(CarouselGridView);

CarouselGridView::CarouselGridView()
{
    numberOfCells = 200;
    targetType = "Carousel";

}

CarouselGridView::~CarouselGridView()
{
}

void CarouselGridView::updateCells() {
    for (int i = 0; i < numberOfCells; i++) {
        Carousel* g = Brain::getInstance()->getCarouselById(i+1);
        if (g != nullptr) {
            //gridButtons[i]->setColour(TextButton::buttonColourId, Colour(127, 127, 127));

            gridButtons[i]->removeColour(TextButton::buttonColourId);
            gridButtons[i]->removeColour(TextButton::textColourOnId);
            gridButtons[i]->removeColour(TextButton::textColourOffId);

            gridButtons[i]->setButtonText(String(i + 1) + "\n"+g->userName->getValue().toString());
        }
        else {
            gridButtons[i]->setButtonText(String(i + 1));
            gridButtons[i]->setColour(TextButton::buttonColourId, Colour(40, 40, 40));
            gridButtons[i]->setColour(TextButton::textColourOnId, Colour(96, 96, 96));
            gridButtons[i]->setColour(TextButton::textColourOffId, Colour(96, 96, 96));

        }
    }
}