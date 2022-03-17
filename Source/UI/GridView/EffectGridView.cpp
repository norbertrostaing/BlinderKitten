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
            gridButtons[i]->setColour(TextButton::buttonColourId, Colour(127, 127, 127));
            gridButtons[i]->setButtonText(String(i + 1) + "\n"+g->userName->getValue().toString());
        }
        else {
            gridButtons[i]->setButtonText(String(i + 1));
            gridButtons[i]->setColour(TextButton::buttonColourId, Colour(63, 63, 63));

        }
    }
}