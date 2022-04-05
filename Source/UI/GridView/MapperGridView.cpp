/*
  ==============================================================================

    MapperGridView.cpp
    Created: 19 Feb 2022 12:19:42am
    Author:  No

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MapperGridView.h"
#include "Brain.h"
#include "Definitions/Mapper/Mapper.h"
#include "Definitions/Mapper/MapperManager.h"

//==============================================================================
MapperGridViewUI::MapperGridViewUI(const String& contentName):
    ShapeShifterContent(MapperGridView::getInstance(), contentName)
{
    
}

MapperGridViewUI::~MapperGridViewUI()
{
}

juce_ImplementSingleton(MapperGridView);

MapperGridView::MapperGridView()
{
    numberOfCells = 200;
    targetType = "Mapper";

}

MapperGridView::~MapperGridView()
{
}

void MapperGridView::updateCells() {
    for (int i = 0; i < numberOfCells; i++) {
        Mapper* g = Brain::getInstance()->getMapperById(i+1);
        if (g != nullptr) {
            gridButtons[i]->setColour(TextButton::buttonColourId, Colour(127, 127, 127));
            gridButtons[i]->setButtonText(String(i + 1) + "\n"+g->userName->getValue().toString());
        }
        else {
            gridButtons[i]->setButtonText(String(i + 1));
            gridButtons[i]->setColour(TextButton::buttonColourId, Colour(63, 63, 63));

        }
    }
}