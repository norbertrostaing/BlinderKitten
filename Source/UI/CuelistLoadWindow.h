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

    Viewport viewport;
    Component btnContainer;

    OwnedArray<TextButton> buttons;
    Array<float> cueIds;
    Cuelist* currentTarget = nullptr;

    TextButton exitBtn;
    void showWindow();
    void fillButtons(Cuelist * c);
    void loadCuelist(Cuelist * c, bool triggerGoWhenSelected = false);
    void buttonClicked(Button*) override;
    bool triggerGo = false;

    int posX = 0;
    int posY = 0;

    juce::CriticalSection buttonsLock;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CuelistLoadWindow)
};
