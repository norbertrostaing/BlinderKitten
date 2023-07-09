/*
  ==============================================================================

    CuelistGridView.cpp
    Created: 19 Feb 2022 12:19:42am
    Author:  No

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CuelistGridView.h"
#include "Brain.h"
#include "Definitions/Cuelist/Cuelist.h"
#include "Definitions/Cuelist/CuelistManager.h"

//==============================================================================
CuelistGridViewUI::CuelistGridViewUI(const String& contentName):
    ShapeShifterContent(CuelistGridView::getInstance(), contentName)
{
    
}

CuelistGridViewUI::~CuelistGridViewUI()
{
}

juce_ImplementSingleton(CuelistGridView);

CuelistGridView::CuelistGridView()
{
    numberOfCells = 200;
    targetType = "Cuelist";

}

CuelistGridView::~CuelistGridView()
{
}

void CuelistGridView::updateCells() {
    for (int i = 0; i < numberOfCells; i++) {
        Cuelist* g = Brain::getInstance()->getCuelistById(i+1);
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

void CuelistGridView::updateButtons()
{
    const MessageManagerLock mmLock;
    for (int i = 0; i < numberOfCells; i++) {
        Cuelist* c = Brain::getInstance()->getCuelistById(i+1);
        if (c != nullptr) {
            if (c->isCuelistOn->boolValue()) {
                gridButtons[i]->setColour(TextButton::buttonColourId, juce::Colour(64, 80, 64));
            }
            else {
                gridButtons[i]->removeColour(TextButton::buttonColourId);
            }
        }
    }

}
