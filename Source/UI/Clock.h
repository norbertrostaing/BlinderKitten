/*
  ==============================================================================

    Clock.h
    Created: 19 Feb 2022 12:19:42am
    Author:  No

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class ClockUI : public ShapeShifterContent
{
public:
    ClockUI(const String& contentName);
    ~ClockUI();

    static ClockUI* create(const String& name) { return new ClockUI(name); }


};


class Clock  : public juce::Component,
    juce::Timer
{
public:
    juce_DeclareSingleton(Clock, true);
    Clock();
    ~Clock() override;

    Label hms;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    void fillText();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Clock)
};
