/*
  ==============================================================================

    PresetGridView.cpp
    Created: 19 Feb 2022 12:19:42am
    Author:  No

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PresetGridView.h"
#include "Brain.h"
#include "Definitions/Preset/Preset.h"
#include "Definitions/Preset/PresetManager.h"

//==============================================================================
PresetGridViewUI::PresetGridViewUI(const String& contentName):
    ShapeShifterContent(PresetGridView::getInstance(), contentName)
{
    
}

PresetGridViewUI::~PresetGridViewUI()
{
}

juce_ImplementSingleton(PresetGridView);

PresetGridView::PresetGridView()
{
    numberOfCells = 200;
    targetType = "preset";

}

PresetGridView::~PresetGridView()
{
}

void PresetGridView::updateCells() {
    for (int i = 0; i < numberOfCells; i++) {
        Preset* g = Brain::getInstance()->getPresetById(i+1);
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