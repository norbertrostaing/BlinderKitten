/*
  ==============================================================================

    CarouselGridView.cpp
    Created: 19 Feb 2022 12:19:42am
    Author:  No

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CarouselGridView.h"
#include "Brain.h"
#include "Definitions/Carousel/Carousel.h"
#include "Definitions/Carousel/CarouselManager.h"

//==============================================================================
CarouselGridViewUI::CarouselGridViewUI(const String& contentName):
    ShapeShifterContent(CarouselGridView::getInstance(), contentName)
{
    
}

CarouselGridViewUI::~CarouselGridViewUI()
{
}

juce_ImplementSingleton(CarouselGridView);

CarouselGridView::CarouselGridView()
{
    numberOfCells = 200;
    targetType = "Carousel";
    CarouselManager::getInstance()->addAsyncManagerListener(this);

}

CarouselGridView::~CarouselGridView()
{
    if (CarouselManager::getInstanceWithoutCreating() != nullptr) CarouselManager::getInstance()->removeAsyncManagerListener(this);
}

void CarouselGridView::updateCells() {
    for (int i = 0; i < numberOfCells; i++) {
        Carousel* g = Brain::getInstance()->getCarouselById(i+1);
        if (g != nullptr) {
            //gridButtons[i]->setColour(TextButton::buttonColourId, Colour(127, 127, 127));

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
    updateButtons();
}

void CarouselGridView::updateButtons()
{
    const MessageManagerLock mmLock;
    for (int i = 0; i < numberOfCells; i++) {
        Carousel* c = Brain::getInstance()->getCarouselById(i + 1);
        if (c != nullptr) {
            if (c->isCarouselOn->boolValue()) {
                gridButtons[i]->setColour(TextButton::buttonColourId, juce::Colour(64, 80, 64));
            }
            else {
                gridButtons[i]->removeColour(TextButton::buttonColourId);
            }
        }
    }
}

void CarouselGridView::showContextMenu(int id)
{
    Carousel* target = Brain::getInstance()->getCarouselById(id);
    if (target != nullptr) {
        PopupMenu p;
        p.addItem("Start", [target]() {target->start(); });
        p.addItem("Stop", [target]() {target->stop(); });
        p.showMenuAsync(PopupMenu::Options(), [this](int result) {});
    }
}

void CarouselGridView::newMessage(const CarouselManager::ManagerEvent& e)
{
    updateCells();
}
