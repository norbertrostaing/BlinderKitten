/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "Command.h"

class CommandManager :
    public BaseManager<Command>
{
public:
    juce_DeclareSingleton(CommandManager, true);

    CommandManager();
    ~CommandManager();

    void addItemInternal(Command* o, var data) override;
    void removeItemInternal(Command* o) override;

    void onContainerParameterChanged(Parameter* p) override;

};