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

    OwnedArray<OwnedArray<TextButton>> buttons;
    OwnedArray<OwnedArray<Slider>> rotaries;
    OwnedArray<Slider> faders;

    void paint (juce::Graphics&) override;
    void resized() override;

    int cols = 1;
    int nRotaries = 1;
    int nAbove = 1;
    int sizeFader = 1;
    int nBelow = 1;

    int page = 1;

    HashMap<TextButton*, VirtualFaderCol*> buttonToVirtualFaderColSlider;
    HashMap<Slider*, VirtualFaderCol*> sliderToVirtualFaderColFader;

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

