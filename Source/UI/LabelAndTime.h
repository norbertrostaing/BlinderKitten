/*
  ==============================================================================

    LabelAndTime.h
    Created: 4 Mar 2022 1:15:59pm
    Author:  No

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
class Cuelist;

//==============================================================================
/*
*/
class LabelAndTimeWindow  : public 
    juce::Component,
    juce::Button::Listener
{
public:
    juce_DeclareSingleton(LabelAndTimeWindow, true);
    LabelAndTimeWindow();
    ~LabelAndTimeWindow() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void applyValues();
    bool newValue(String input, float &currentVal);

    void closeWindow();
    bool windowOpened = false;

    TextEditor::LengthAndCharacterRestriction filter;

    TextButton validBtn;
    TextButton exitBtn;

    Label nameLabel;
    TextEditor nameEdit;

    Label htpUpLabel;
    Label htpDownLabel;
    Label ltpLabel;
    Label delayLabel;
    Label fadeLabel;

    TextEditor htpUpDelayEdit;
    TextEditor htpDownDelayEdit;
    TextEditor ltpDelayEdit;
    TextEditor htpUpFadeEdit;
    TextEditor htpDownFadeEdit;
    TextEditor ltpFadeEdit;

    void showWindow();
    void buttonClicked(Button*);

    int posX = 0;
    int posY = 0;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LabelAndTimeWindow)
};
