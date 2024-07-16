/*
  ==============================================================================

    VirtualButtonGrid.h
    Created: 19 Feb 2022 12:19:42am
    Author:  No

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class VirtualButton;
class BKEngine;
//==============================================================================
/*
*/
class VirtualButtonGridUI : public ShapeShifterContent
{
public:
    VirtualButtonGridUI(const String& contentName);
    ~VirtualButtonGridUI();

    static VirtualButtonGridUI* create(const String& name) { return new VirtualButtonGridUI(name); }
};


class VirtualButtonGrid  : 
    public juce::Component,
    public BaseItem,
    public Button::Listener
{
public:
    juce_DeclareSingleton(VirtualButtonGrid, true);
    VirtualButtonGrid();
    ~VirtualButtonGrid() override;

    BKEngine* engine = nullptr;

    void initCells();
    void fillCells();

    TextButton pagePlusBtn;
    TextButton pageDisplayBtn;
    TextButton pageMinusBtn;
    OwnedArray<TextButton> gridButtons;

    void paint (juce::Graphics&) override;
    void resized() override;

    int rows = 1;
    int cols = 1;
    int page = 1;

    HashMap<TextButton*, VirtualButton*> buttonToVirtualButton;

    void buttonClicked(juce::Button* button) override;
    void buttonStateChanged(juce::Button* button) override;

    void buttonPressedDown(juce::TextButton* button);
    void buttonPressedUp(juce::TextButton* button);

    void editCell(int id);
    void deleteCell(int id);
    void moveCell(int idFrom, int idTo);
    void copyCell(int idFrom, int idTo);

    void goToPage(int n);

    VirtualButton* getVirtualButton(int index, bool create = false);

    void updateButtons( bool forceFeedbacks = false);
    void resetFeedbacks(bool onlySetToZero = false);


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VirtualButtonGrid)
};

