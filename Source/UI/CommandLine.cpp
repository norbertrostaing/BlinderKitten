/*
  ==============================================================================

    CommandLine.cpp
    Created: 19 Feb 2022 12:19:42am
    Author:  No

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CommandLine.h"

//==============================================================================
CommandLineUI::CommandLineUI(const String& contentName):
    ShapeShifterContent(CommandLine::getInstance(), contentName)
{
    
}

CommandLineUI::~CommandLineUI()
{
}

juce_ImplementSingleton(CommandLine);

CommandLine::CommandLine()
{
    addAndMakeVisible(test);
    test.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
}

CommandLine::~CommandLine()
{
}

void CommandLine::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);

    int r = Random::getSystemRandom().nextInt(100);
    g.drawText ("CommandLine "+String(r), getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text


}

void CommandLine::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    int h = getHeight();
    int w = getWidth();
    if (h > w) { // portrait
        int s = jmin(w-70, 200);
        s = jmax(100,s);
        test.setBounds(10, 0, s, s);
        test.setTextBoxStyle(Slider::TextBoxRight, false, 60, 20);
    }
    else {
        test.setBounds(0, 0, h, h);
        test.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
    }
}
