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


GridViewButton::GridViewButton()
{
}

GridViewButton::~GridViewButton()
{
}

void GridViewButton::paint(juce::Graphics& g)
{
    TextButton::paint(g);
    g.drawFittedText(String(id), getLocalBounds(), Justification::topRight,1);
}




juce_ImplementSingleton(GridView);

GridView::GridView():
    Component(),
    scroll(true),
    ScrollBar::Listener()
{
    initArrays();
    addMouseListener(this, true);
    addAndMakeVisible(scroll);
    scroll.addListener(this);
    resized();


    // addAndMakeVisible(test);
    // test.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
}

GridView::~GridView()
{
}

void GridView::initArrays() {
    gridButtons.clear();
    for (int i = 0; i < numberOfCells; i++) {
        GridViewButton* b = new GridViewButton();
        b->id = i+1;
        gridButtons.add(b);
        b->addListener(this);
        addAndMakeVisible(b);
    }
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
    int btnSizeH = 50 * scale;
    scroll.setBounds(w-scrollWidth,0,scrollWidth,h);
    w = w-scrollWidth;
   
    int nByRow = w/ btnSizeH;
    if (nByRow == 0) { return; }

    nByRow = engine->gridCols->getValue();

    int btnSizeW = w/nByRow;
    int margin = (w%btnSizeW)/2;

    int x = 0;
    int y = -1;
    int maxY = 0;
    //int currentX = 0;
    int lastValidId = 0;
    for (int i = 0; i < numberOfCells; i++) {
        if (completeGridMode) {
            x = i % nByRow;
            y = i / nByRow;
            x = margin + (x * btnSizeW);
            maxY = (y + 1) * btnSizeH;
            y = y * btnSizeH - scroll.getCurrentRangeStart();
            gridButtons[i]->setBounds(x, y, btnSizeW, btnSizeH);
        }
        else {
            if (validCells.contains(i)) {
                gridButtons[i]->setVisible(true);
                if (i == lastValidId + 1) {
                    x = (x+1) % nByRow;
                    if (x == 0) {y+=1;}
                }
                else {
                    x = 0;
                    y += 1;
                }
                int drawX = margin + (x * btnSizeW);
                int drawY = y * btnSizeH - scroll.getCurrentRangeStart();
                gridButtons[i]->setBounds(drawX, drawY, btnSizeW, btnSizeH);
                maxY = (y + 1) * btnSizeH;
                lastValidId = i;
            }
            else {
                gridButtons[i]->setVisible(false);
            }
        }
        
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
    int id = gridButtons.indexOf((GridViewButton*)button);
    cellClicked(id+1);
    UserInputManager::getInstance()->resetFocus();
}

void GridView::cellClicked(int id) {
    UserInputManager::getInstance()->gridViewCellPressed(targetType, id);
}

void GridView::updateCells() {
    
}

