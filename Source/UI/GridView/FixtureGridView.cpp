/*
  ==============================================================================

    FixtureGridView.cpp
    Created: 19 Feb 2022 12:19:42am
    Author:  No

  ==============================================================================
*/

#include <JuceHeader.h>
#include "FixtureGridView.h"
#include "Brain.h"
#include "Definitions/Fixture/Fixture.h"
#include "Definitions/Fixture/FixtureManager.h"

//==============================================================================
FixtureGridViewUI::FixtureGridViewUI(const String& contentName) :
    ShapeShifterContent(FixtureGridView::getInstance(), contentName)
{

}

FixtureGridViewUI::~FixtureGridViewUI()
{
}

juce_ImplementSingleton(FixtureGridView);

FixtureGridView::FixtureGridView()
{
    targetType = "Fixture";
    completeGridMode = false;

}

FixtureGridView::~FixtureGridView()
{
}

void FixtureGridView::updateCells() {
    validCells.clear();
    for (int i = 0; i < numberOfCells; i++) {
        Fixture* g = Brain::getInstance()->getFixtureById(i + 1);
        if (g != nullptr) {
            gridButtons[i]->setColour(TextButton::buttonColourId, Colour(127, 127, 127));
            gridButtons[i]->setButtonText(String(i + 1) + "\n" + g->userName->getValue().toString());
            validCells.add(i);
        }
        else {
            gridButtons[i]->setButtonText(String(i + 1));
            gridButtons[i]->setColour(TextButton::buttonColourId, Colour(63, 63, 63));

        }
    }
}