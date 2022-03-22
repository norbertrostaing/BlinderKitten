/*
  ==============================================================================

    GridView.h
    Created: 19 Feb 2022 12:19:42am
    Author:  No

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
class BKEngine;
//==============================================================================
/*
*/
class GridViewUI : public ShapeShifterContent
{
public:
    GridViewUI(const String& contentName);
    ~GridViewUI();

    static GridViewUI* create(const String& name) { return new GridViewUI(name); }

};


class GridView  : 
    public juce::Component,
    public BaseItem,
    public ScrollBar::Listener, 
    public Button::Listener
{
public:
    juce_DeclareSingleton(GridView, true);
    GridView();
    ~GridView() override;

    BKEngine* engine = nullptr;
    OwnedArray<TextButton> gridButtons;
    ScrollBar scroll;

    bool completeGridMode = true;

    void paint (juce::Graphics&) override;
    void resized() override;

    int numberOfCells = 200;
    String targetType;
    Array<int> validCells;

    void scrollBarMoved(ScrollBar* scrollBarThatHasMoved, double newRangeStart );
    void mouseWheelMove(const MouseEvent& event,const MouseWheelDetails& wheel);
    void buttonClicked(juce::Button* button) override;
    virtual void cellClicked(int id);
    virtual void updateCells();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GridView)
};

