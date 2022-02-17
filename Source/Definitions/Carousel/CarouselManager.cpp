#include "Carousel.h"
#include "CarouselManager.h"

/*
  ==============================================================================

    ObjectManager.cpp
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

juce_ImplementSingleton(CarouselManager);

int compare(Carousel* A, Carousel* B) {
    return (int)A->id->getValue() - (int)B->id->getValue();
}

CarouselManager::CarouselManager() :
    BaseManager("Carousel")
    {
    itemDataType = "Carousel";
    selectItemWhenCreated = true;
    comparator.compareFunc = compare;
}

CarouselManager::~CarouselManager()
{
    // stopThread(1000);
}


void CarouselManager::addItemInternal(Carousel* o, var data)
{
    reorderItems();
}

void CarouselManager::removeItemInternal(Carousel* o)
{
    // o->removeObjectListener(this);
}


void CarouselManager::onContainerParameterChanged(Parameter* p)
{
   // if (p == lockUI) for (auto& i : items) i->isUILocked->setValue(lockUI->boolValue());
}

