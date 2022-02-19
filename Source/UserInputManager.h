/*
  ==============================================================================

    UserInputManager.h
    Created: 14 Nov 2021 10:42:56pm
    Author:  No

  ==============================================================================
*/
#pragma once

#include "JuceHeader.h"

class Programmer;
class Command;
//#include "Definitions/Programmer/Programmer.h"

class UserInputManager:
    public OSCRemoteControl::RemoteControlListener
{
public:
    juce_DeclareSingleton(UserInputManager, true);

    StringArray currentCommand;
    Command* targetCommand;

    UserInputManager();
    ~UserInputManager();

	void processInput(String s);
    void processCommand();
    void processMessage(const OSCMessage& m) override;

    void encoderValueChanged(int index, float absVal);

    void commandSelectionChanged(Command* c);

};
