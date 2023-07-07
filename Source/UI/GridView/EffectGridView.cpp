/*
  ==============================================================================

    EffectGridView.cpp
    Created: 19 Feb 2022 12:19:42am
    Author:  No

  ==============================================================================
*/

#include <JuceHeader.h>
#include "EffectGridView.h"
#include "Brain.h"
#include "Definitions/Effect/Effect.h"
#include "Definitions/Effect/EffectManager.h"

//==============================================================================
EffectGridViewUI::EffectGridViewUI(const String& contentName):
    ShapeShifterContent(EffectGridView::getInstance(), contentName)
{
    
}

EffectGridViewUI::~EffectGridViewUI()
{
}

juce_ImplementSingleton(EffectGridView);

EffectGridView::EffectGridView()
{
    numberOfCells = 200;
    targetType = "Effect";

}

EffectGridView::~EffectGridView()
{
}

void EffectGridView::updateCells() {
    for (int i = 0; i < numberOfCells; i++) {
        Effect* g = Brain::getInstance()->getEffectById(i+1);
        if (g != nullptr) {
            gridButtons[i]->removeColour(TextButton::buttonColourId);
            gridButtons[i]->removeColour(TextButton::textColourOnId);
            gridButtons[i]->removeColour(TextButton::textColourOffId);

            gridButtons[i]->setButtonText(String(i + 1) + "\n" + g->userName->getValue().toString());
        }
        else {
            gridButtons[i]->setButtonText(String(i + 1));
            gridButtons[i]->setColour(TextButton::buttonColourId, Colour(40, 40, 40));
            gridButtons[i]->setColour(TextButton::textColourOnId, Colour(96, 96, 96));
            gridButtons[i]->setColour(TextButton::textColourOffId, Colour(96, 96, 96));

        }
    }
}

void EffectGridView::updateButtons()
{
    for (int i = 0; i < numberOfCells; i++) {
        Effect* c = Brain::getInstance()->getEffectById(i + 1);
        if (c != nullptr) {
            if (c->isEffectOn->boolValue()) {
                gridButtons[i]->setColour(TextButton::buttonColourId, juce::Colour(64, 80, 64));
            }
            else {
                gridButtons[i]->removeColour(TextButton::buttonColourId);
            }
        }
    }

}
