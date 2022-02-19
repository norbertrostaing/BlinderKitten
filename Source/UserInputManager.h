/*
  ==============================================================================

    UserInputManager.h
    Created: 14 Nov 2021 10:42:56pm
    Author:  No

  ==============================================================================
*/
#pragma once

#include "JuceHeader.h"
#include "Definitions/Programmer/Programmer.h"

class UserInputManager:
    public OSCRemoteControl::RemoteControlListener
{
public:
    juce_DeclareSingleton(UserInputManager, true);

    StringArray currentCommand;

    UserInputManager();
    ~UserInputManager();

	void processInput(String s);
    void processCommand();
    void processMessage(const OSCMessage& m) override;

};
