/*
  ==============================================================================

    Clock.cpp
    Created: 19 Feb 2022 12:19:42am
    Author:  No

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Clock.h"

//==============================================================================
ClockUI::ClockUI(const String& contentName):
    ShapeShifterContent(Clock::getInstance(), contentName)
{

}

ClockUI::~ClockUI()
{
}

juce_ImplementSingleton(Clock);

Clock::Clock()
{
    addAndMakeVisible(hms);
    hms.setJustificationType(juce::Justification::centred);
    fillText();
    resized();
    startTimerHz(10);
}

Clock::~Clock()
{
    stopTimer();
}

void Clock::paint (juce::Graphics& g)
{
}

void Clock::resized()
{
    hms.setBounds(getLocalBounds());
    Font f = Font(10, Font::plain);
    float h = f.getHeight();
    float w = f.getStringWidth(hms.getText());

    float sizeH = 10 * getHeight()/h;
    float sizeW = 10* getWidth() / w;

    hms.setFont(Font(jmin(sizeW,sizeH), Font::plain));
}

void Clock::timerCallback()
{
    fillText();
}

void Clock::fillText()
{
    String hour = Time::getCurrentTime().formatted("%H:%M:%S");
    if (hour != hms.getText()) {
        hms.setText(hour, juce::NotificationType::sendNotification);
    }
}
