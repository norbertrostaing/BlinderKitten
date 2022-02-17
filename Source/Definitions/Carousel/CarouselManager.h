/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "Carousel.h"

class CarouselManager :
    public BaseManager<Carousel>
{
public:
    juce_DeclareSingleton(CarouselManager, true);

    CarouselManager();
    ~CarouselManager();

    void addItemInternal(Carousel* o, var data) override;
    void removeItemInternal(Carousel* o) override;

    void onContainerParameterChanged(Parameter* p) override;

};