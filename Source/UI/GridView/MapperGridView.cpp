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
#include "DataTransferManager/DataTransferManager.h"

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
    MapperManager::getInstance()->addAsyncManagerListener(this);

}

MapperGridView::~MapperGridView()
{
    if (MapperManager::getInstanceWithoutCreating() != nullptr) MapperManager::getInstance()->removeAsyncManagerListener(this);
}

void MapperGridView::updateCells() {
    for (int i = 0; i < numberOfCells; i++) {
        Mapper* g = Brain::getInstance()->getMapperById(i+1);
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

void MapperGridView::updateButtons()
{
    const MessageManagerLock mmLock;
    for (int i = 0; i < numberOfCells; i++) {
        Mapper* c = Brain::getInstance()->getMapperById(i + 1);
        if (c != nullptr) {
            if (c->isMapperOn->boolValue()) {
                gridButtons[i]->setColour(TextButton::buttonColourId, juce::Colour(64, 80, 64));
            }
            else {
                gridButtons[i]->removeColour(TextButton::buttonColourId);
            }
        }
    }
}


void MapperGridView::showContextMenu(int id)
{
    Mapper* target = Brain::getInstance()->getMapperById(id);
    PopupMenu p;
    if (target != nullptr) {
        p.addItem("Start", [target]() {target->start(); });
        p.addItem("Stop", [target]() {target->stop(); });
    }
    else {
        p.addItem("Add", [id]() {DataTransferManager::getInstance()->editObject("mapper", id); });
    }
    p.showMenuAsync(PopupMenu::Options(), [this](int result) {});

}

void MapperGridView::newMessage(const MapperManager::ManagerEvent& e)
{
    updateCells();
}
