/*
  ==============================================================================

    CVProgrammerManagerUI.h
    Created: 22 Feb 2018 3:42:48pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Programmer.h"

class ProgrammerManagerUI :
	public BaseManagerShapeShifterUI<BaseManager<Programmer>, Programmer, BaseItemUI<Programmer>>
{
public:
	ProgrammerManagerUI(const String &contentName);
	~ProgrammerManagerUI();

	static ProgrammerManagerUI * create(const String &name) { return new ProgrammerManagerUI(name); }
};