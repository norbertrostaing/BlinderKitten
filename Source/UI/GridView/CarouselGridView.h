/*
  ==============================================================================

    CarouselGridView.h
    Created: 19 Feb 2022 12:19:42am
    Author:  No

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GridView.h"
//==============================================================================
/*
*/
class CarouselGridViewUI : public ShapeShifterContent
{
public:
    CarouselGridViewUI(const String& contentName);
    ~CarouselGridViewUI();

    static CarouselGridViewUI* create(const String& name) { return new CarouselGridViewUI(name); }


};


class CarouselGridView  : 
    public GridView
{
public:
    juce_DeclareSingleton(CarouselGridView, true);
    CarouselGridView();
    ~CarouselGridView() override;
    
    void updateCells() override;
    void updateButtons();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CarouselGridView)
};


