/*
  ==============================================================================

    CuelistLoadWindow.h
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
class CuelistLoadWindow  : public 
    juce::Component,
    juce::Button::Listener
    
{
public:
    juce_DeclareSingleton(CuelistLoadWindow, true);
    CuelistLoadWindow();
    ~CuelistLoadWindow() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void closeWindow();
    bool windowOpened = false;

    OwnedArray<TextButton> buttons;
    Array<float> cueIds;
    Cuelist* currentTarget = nullptr;

    TextButton exitBtn;
    void fillButtons(Cuelist * c);
    void loadCuelist(Cuelist * c, bool triggerGoWhenSelected = false);
    void buttonClicked(Button*);
    bool triggerGo = false;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CuelistLoadWindow)
};
