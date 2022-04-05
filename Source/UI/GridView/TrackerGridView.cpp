/*
  ==============================================================================

    TrackerGridView.cpp
    Created: 19 Feb 2022 12:19:42am
    Author:  No

  ==============================================================================
*/

#include <JuceHeader.h>
#include "TrackerGridView.h"
#include "Brain.h"
#include "Definitions/Tracker/Tracker.h"
#include "Definitions/Tracker/TrackerManager.h"

//==============================================================================
TrackerGridViewUI::TrackerGridViewUI(const String& contentName):
    ShapeShifterContent(TrackerGridView::getInstance(), contentName)
{
    
}

TrackerGridViewUI::~TrackerGridViewUI()
{
}

juce_ImplementSingleton(TrackerGridView);

TrackerGridView::TrackerGridView()
{
    numberOfCells = 200;
    targetType = "Tracker";

}

TrackerGridView::~TrackerGridView()
{
}

void TrackerGridView::updateCells() {
    for (int i = 0; i < numberOfCells; i++) {
        Tracker* g = Brain::getInstance()->getTrackerById(i+1);
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