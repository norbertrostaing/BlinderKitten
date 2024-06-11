/*
  ==============================================================================

    InputPanel.h
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
class InputPanelUI : public ShapeShifterContent
{
public:
    InputPanelUI(const String& contentName);
    ~InputPanelUI();

    static InputPanelUI* create(const String& name) { return new InputPanelUI(name); }


};


class InputPanel  : public 
    juce::Component,
    juce::Button::Listener,
    juce::Slider::Listener
{
public:
    juce_DeclareSingleton(InputPanel, true);
    InputPanel();
    ~InputPanel() override;

    BKEngine * engine = nullptr;

    TextButton btnFixture;
    TextButton btnGroup;
    TextButton btnSub;
    TextButton btnCuelist;
    TextButton btnCue;
    TextButton btnPreset;
    TextButton btnEffect;
    TextButton btnCarousel;
    TextButton btn1;
    TextButton btn2;
    TextButton btn3;
    TextButton btn4;
    TextButton btn5;
    TextButton btn6;
    TextButton btn7;
    TextButton btn8;
    TextButton btn9;
    TextButton btn0;
    TextButton btnPlus;
    TextButton btnMinus;
    TextButton btnDot;
    TextButton btnThru;
    TextButton btnBackspace;
    TextButton btnEnter;
    TextButton btnClear;
    TextButton btnRecord;
    TextButton btnEdit;
    TextButton btnCopy;
    TextButton btnDelete;
    TextButton btnAssistant;

    Slider grandMaster;
    TextButton blackoutBtn;

    void paint (juce::Graphics&) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(Slider* slider) override;

    void setGrandMaster(float value, String origin);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InputPanel)
};
