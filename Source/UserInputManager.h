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
class Programmer;

//#include "Definitions/Programmer/Programmer.h"

class UserInputManager:
    public OSCRemoteControl::RemoteControlListener
{
public:
    juce_DeclareSingleton(UserInputManager, true);

    StringArray currentCommand;
    Command* targetCommand;

    OwnedArray<StringArray *> otherCommands;
    OwnedArray<Command*> otherTargetCommands;

    Programmer* currentProgrammer;

    UserInputManager();
    ~UserInputManager();

	void processInput(String s);
    void processMessage(const OSCMessage& m) override;

    void encoderValueChanged(int index, float absVal);
    HashMap<int, ChannelType*> encoderChannels;

    void commandSelectionChanged(Command* c);
    void commandValueChanged(Command* c);

    void redrawEncoders();
    void updateCommandLine();

    String toUserText(String s);
    Programmer* getProgrammer();

    static int backspaceOnInt(var);
    static float backspaceOnFloat(var);
    void gridViewCellPressed(String type, int id);

};
