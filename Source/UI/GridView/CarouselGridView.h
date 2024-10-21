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
#include "Carousel/CarouselManager.h"
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
    public GridView,
    public CarouselManager::AsyncListener
{
public:
    juce_DeclareSingleton(CarouselGridView, true);
    CarouselGridView();
    ~CarouselGridView() override;
    
    void updateCells() override;
    void updateButtons();
    void showContextMenu(int id);
    void newMessage(const CarouselManager::ManagerEvent& e) override;


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CarouselGridView)
};


