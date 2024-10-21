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
    targetType = "Group";
    GroupManager::getInstance()->addAsyncManagerListener(this);

}

GroupGridView::~GroupGridView()
{
    if (GroupManager::getInstanceWithoutCreating() != nullptr) GroupManager::getInstance()->removeAsyncManagerListener(this);
}

void GroupGridView::updateCells() {
    validCells.clear();
    for (int i = 0; i < numberOfCells; i++) {
        Group* g = Brain::getInstance()->getGroupById(i+1);
        if (g != nullptr) {
            validCells.add(i);
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

void GroupGridView::newMessage(const GroupManager::ManagerEvent& e)
{
    updateCells();
}
