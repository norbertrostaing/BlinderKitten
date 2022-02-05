/*
  ==============================================================================

    CVGroupManagerUI.h
    Created: 22 Feb 2018 3:42:48pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Command.h"

class CommandManagerUI :
	public BaseManagerShapeShifterUI<BaseManager<Command>, Command, BaseItemUI<Command>>
{
public:
	CommandManagerUI(const String &contentName);
	~CommandManagerUI();

	static CommandManagerUI * create(const String &name) { return new CommandManagerUI(name); }
};