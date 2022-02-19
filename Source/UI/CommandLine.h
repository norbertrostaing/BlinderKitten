/*
  ==============================================================================

    CommandLine.h
    Created: 19 Feb 2022 12:19:42am
    Author:  No

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class CommandLineUI : public ShapeShifterContent
{
public:
    CommandLineUI(const String& contentName);
    ~CommandLineUI();

    static CommandLineUI* create(const String& name) { return new CommandLineUI(name); }


};


class CommandLine  : public juce::Component
{
public:
    juce_DeclareSingleton(CommandLine, true);
    CommandLine();
    ~CommandLine() override;

    Slider test;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CommandLine)
};
