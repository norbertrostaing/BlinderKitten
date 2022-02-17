/*
  ==============================================================================

    CarouselManagerUI.h
    Created: 22 Feb 2018 3:42:48pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Carousel.h"

class CarouselManagerUI :
	public BaseManagerShapeShifterUI<BaseManager<Carousel>, Carousel, BaseItemUI<Carousel>>
{
public:
	CarouselManagerUI(const String &contentName);
	~CarouselManagerUI();

	static CarouselManagerUI * create(const String &name) { return new CarouselManagerUI(name); }
};