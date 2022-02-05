/*
  ==============================================================================

    ActionManager.h
    Created: 12 Oct 2020 10:55:26am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class ActionFactory :
    public Factory<Action>
{
public:
    juce_DeclareSingleton(ActionFactory, true)
    ActionFactory();

};

class ActionManager :
    public BaseManager<Action>
{
public:
    ActionManager(const String &name = "Actions");
    ~ActionManager();

    void triggerAll();
    void setValueAll(var value);
};