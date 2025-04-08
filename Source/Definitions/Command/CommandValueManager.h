/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "CommandValue.h"
// #include "../ChannelFamily/ChannelType/ChannelType.h"

class CommandValueManager :
    public BaseManager<CommandValue>
{
public:
    CommandValueManager(const juce::String& name = "CommandValue");
    ~CommandValueManager();

    bool shouldShowStepSize = false;
    void showStepSize(bool should);
    void addItemInternal(CommandValue*, juce::var data) override;
};

