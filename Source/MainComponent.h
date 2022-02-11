#pragma once

#include <JuceHeader.h>


#include "MainComponent.h"
#include "Definitions/ChannelFamily/ChannelFamilyManagerUI.h"
#include "BKEngine.h"


ApplicationProperties& getAppProperties();
ApplicationCommandManager& getCommandManager();

class MainContentComponent;
class BKEngine;

class BKMenuBarComponent :
    public Component,
    public Thread,
    public Timer
{
public:
    BKMenuBarComponent(MainContentComponent* mainComp, BKEngine* engine);
    ~BKMenuBarComponent();

#if !JUCE_MAC
    MenuBarComponent menuBarComp;
#endif
    void paint(Graphics& g) override;
    void resized() override;

    void timerCallback() override;
    void run() override;

};


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent :
    public OrganicMainContentComponent
    {
public:
    //==============================================================================
    MainContentComponent();
    ~MainContentComponent() override;
    
    void init() override;


private:
    //==============================================================================
    // Your private member variables go here...


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
