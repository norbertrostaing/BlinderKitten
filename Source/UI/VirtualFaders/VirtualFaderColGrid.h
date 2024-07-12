/*
  ==============================================================================

    VirtualFaderColGrid.h
    Created: 19 Feb 2022 12:19:42am
    Author:  No

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class VirtualFaderCol;
class VirtualFaderButton;
class VirtualFaderSlider;
class BKEngine;
//==============================================================================
/*
*/
class VirtualFaderColGridUI : public ShapeShifterContent
{
public:
    VirtualFaderColGridUI(const String& contentName);
    ~VirtualFaderColGridUI();

    static VirtualFaderColGridUI* create(const String& name) { return new VirtualFaderColGridUI(name); }
};


class VirtualFaderColGrid  : 
    public juce::Component,
    public BaseItem,
    public Button::Listener,
    public Slider::Listener
{
public:
    juce_DeclareSingleton(VirtualFaderColGrid, true);
    VirtualFaderColGrid();
    ~VirtualFaderColGrid() override;

    BKEngine* engine = nullptr;

    void initCells();
    void fillCells();

    TextButton pagePlusBtn;
    TextButton pageDisplayBtn;
    TextButton pageMinusBtn;

    OwnedArray<OwnedArray<TextButton>> aboveButtons;
    OwnedArray<OwnedArray<TextButton>> belowButtons;
    OwnedArray<OwnedArray<Slider>> rotaries;
    OwnedArray<OwnedArray<Label>> rotaryLabels;
    OwnedArray<Slider> faders;
    OwnedArray<Label>faderLabels;
    OwnedArray<Label>columnLabels;

    void paint (juce::Graphics&) override;
    void resized() override;

    int cols = 1;
    int nRotaries = 1;
    int nAbove = 1;
    int sizeFader = 1;
    int nBelow = 1;

    int page = 1;

    HashMap<TextButton*, VirtualFaderButton*> buttonToVFB;
    HashMap<Slider*, VirtualFaderSlider*> sliderToVFS;
    HashMap<TextButton*, int> buttonColumnIndex;
    HashMap<Slider*, int> sliderColumnIndex;
    HashMap<int, VirtualFaderCol*> columnToVFC;

    HashMap<TextButton*, int> buttonToIndex;
    HashMap<int, TextButton*> indexToButton;

    HashMap<Slider*, int> sliderToIndex;
    HashMap<int, Slider*> indexToSlider;

    HashMap<TextButton*, bool> buttonIsAbove;

    void buttonClicked(juce::Button* button) override;
    void buttonStateChanged(juce::Button* button) override;
    void sliderValueChanged(Slider* slider) override;
    void mouseDown(const MouseEvent& m);

    void buttonPressedDown(juce::TextButton* button);
    void buttonPressedUp(juce::TextButton* button);

    void sliderClicked(Slider* s);

    void editCol(int id);
    void deleteCol(int id);
    void moveCol(int idFrom, int idTo);
    void copyCol(int idFrom, int idTo);

    void editElmt(int id);
    void deleteElmt(int id);
    void moveElmt(int idFrom, int idTo);
    void copyElmt(int idFrom, int idTo);

    VirtualFaderCol* getVirtualFaderCol(int index, bool create = false);
    VirtualFaderButton* getVirtualFaderButton(int index, bool create = false);
    VirtualFaderSlider* getVirtualFaderSlider(int index, bool create = false);
    void updateSlidersValues(bool forceAll = false);

    void goToPage(int n);

    void resetFeedbacks();
    void updateButtons(bool forceFeedbacks = false);



private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VirtualFaderColGrid)
};

