/*
  ==============================================================================

    GroupGridView.cpp
    Created: 19 Feb 2022 12:19:42am
    Author:  No

  ==============================================================================
*/

#include <JuceHeader.h>
#include "GroupGridView.h"
#include "Brain.h"
#include "Definitions/Group/Group.h"
#include "Definitions/Group/GroupManager.h"

//==============================================================================
GroupGridViewUI::GroupGridViewUI(const String& contentName):
    ShapeShifterContent(GroupGridView::getInstance(), contentName)
{
    
}

GroupGridViewUI::~GroupGridViewUI()
{
}

juce_ImplementSingleton(GroupGridView);

GroupGridView::GroupGridView()
{
    numberOfCells = 200;
    targetType = "group";

}

GroupGridView::~GroupGridView()
{
}

void GroupGridView::updateCells() {
    for (int i = 0; i < numberOfCells; i++) {
        Group* g = Brain::getInstance()->getGroupById(i+1);
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