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

    void buttonClicked(juce::Button* button) override;
    void buttonStateChanged(juce::Button* button) override;
    void sliderValueChanged(Slider* slider) override;

    void buttonPressedDown(juce::TextButton* button);
    void buttonPressedUp(juce::TextButton* button);

    void editCell(int id);
    void deleteCell(int id);
    void moveCell(int idFrom, int idTo);
    void copyCell(int idFrom, int idTo);




private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VirtualFaderColGrid)
};

