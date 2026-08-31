/*
  ==============================================================================

    BKColorPicker.h
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
class BKColorPickerUI : public ShapeShifterContent
{
public:
    BKColorPickerUI(const String& contentName);
    ~BKColorPickerUI();

    static BKColorPickerUI* create(const String& name) { return new BKColorPickerUI(name); }


};


class BKColorPicker  : public 
    juce::Component,
    juce::Button::Listener,
    juce::Slider::Listener
{
public:
    juce_DeclareSingleton(BKColorPicker, true);
    BKColorPicker();
    ~BKColorPicker() override;

    BKEngine * engine = nullptr;

    ImageComponent background;

    double margin = 10;

    void paint (juce::Graphics&) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(Slider* slider) override;

    void mouseDown(const MouseEvent& e) override;
    void mouseDrag(const MouseEvent& e) override;
    void mouseSetColor(double x, double y);


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKColorPicker)
};
