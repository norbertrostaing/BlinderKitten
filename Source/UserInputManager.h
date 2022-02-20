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
class ChannelType;
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
    HashMap<int, ChannelType*> encoderChannels;

    void commandSelectionChanged(Command* c);
    void commandValueChanged(Command* c);

    void redrawEncoders();

};
