/*
  ==============================================================================

    GridView.cpp
    Created: 19 Feb 2022 12:19:42am
    Author:  No

  ==============================================================================
*/

#include <JuceHeader.h>
#include "GridView.h"
#include "UserInputManager.h"
#include "BKEngine.h"

//==============================================================================
GridViewUI::GridViewUI(const String& contentName):
    ShapeShifterContent(GridView::getInstance(), contentName)
{
    
}

GridViewUI::~GridViewUI()
{
}

juce_ImplementSingleton(GridView);

GridView::GridView():
    BaseItem("GridView"),
    scroll(true),
    ScrollBar::Listener()
{
    addMouseListener(this, true);
    addAndMakeVisible(scroll);
    scroll.addListener(this);
    for (int i = 0; i < numberOfCells; i++) {
        TextButton* b = new TextButton();
        gridButtons.add(b);
        b->addListener(this);
        addAndMakeVisible(b);
    }

    resized();


    // addAndMakeVisible(test);
    // test.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
}

GridView::~GridView()
{
}

void GridView::paint (juce::Graphics& g)
{
}

void GridView::resized()
{
    float scale = 1;
    if (engine != nullptr && engine->gridScale != nullptr) {
        scale = engine->gridScale->getValue();
    }
    int h = getHeight();
    int w = getWidth();
    int scrollWidth = 10;
    int btnSizeH = 50*scale;
    scroll.setBounds(w-scrollWidth,0,scrollWidth,h);
    w = w-scrollWidth;
   
    int nByRow = w/ btnSizeH;
    if (nByRow == 0) { return; }

    int btnSizeW = w/nByRow;
    int margin = (w%btnSizeW)/2;

    int x = 0;
    int y = 0;
    int maxY = 0;
    for (int i = 0; i < numberOfCells; i++) {
        x = i%nByRow;
        y = i/nByRow;
        x = margin + (x * btnSizeW);
        y = y* btnSizeH - scroll.getCurrentRangeStart();
        gridButtons[i]->setBounds(x,y, btnSizeW, btnSizeH);
        maxY = (y+1)* btnSizeH;
    }
    scroll.setCurrentRange(scroll.getCurrentRangeStart(), h,juce::dontSendNotification);
    scroll.setRangeLimits(0, maxY, juce::dontSendNotification);
}


void GridView::mouseWheelMove(const MouseEvent& event,const MouseWheelDetails& wheel) {
    scroll.setCurrentRangeStart(scroll.getCurrentRangeStart() - (10*wheel.deltaY));
    resized();
}

void GridView::scrollBarMoved(ScrollBar* scrollBarThatHasMoved, double newRangeStart) {
    resized();
}

void GridView::buttonClicked(juce::Button* button) {
    int id = gridButtons.indexOf((TextButton*)button);
    cellClicked(id+1);
}

void GridView::cellClicked(int id) {
    UserInputManager::getInstance()->gridViewCellPressed(targetType, id);
}

void GridView::updateCells() {
    
}