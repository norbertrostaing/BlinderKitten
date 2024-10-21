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
    PresetManager::getInstance()->addAsyncManagerListener(this);

}

PresetGridView::~PresetGridView()
{
    if (PresetManager::getInstanceWithoutCreating() != nullptr) PresetManager::getInstance()->removeAsyncManagerListener(this);
}

void PresetGridView::updateCells() {
    for (int i = 0; i < numberOfCells; i++) {
        Preset* g = Brain::getInstance()->getPresetById(i+1);
        if (g != nullptr) {
            gridButtons[i]->removeColour(TextButton::buttonColourId);
            gridButtons[i]->removeColour(TextButton::textColourOnId);
            gridButtons[i]->removeColour(TextButton::textColourOffId);

            gridButtons[i]->setButtonText(g->userName->getValue().toString());
        }
        else {
            gridButtons[i]->setButtonText("");
            gridButtons[i]->setColour(TextButton::buttonColourId, Colour(40, 40, 40));
            gridButtons[i]->setColour(TextButton::textColourOnId, Colour(96, 96, 96));
            gridButtons[i]->setColour(TextButton::textColourOffId, Colour(96, 96, 96));

        }
    }
}

void PresetGridView::newMessage(const PresetManager::ManagerEvent& e)
{
    updateCells();
}
