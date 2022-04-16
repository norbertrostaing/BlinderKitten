/*
  ==============================================================================

    CVGroupManagerUI.h
    Created: 22 Feb 2018 3:42:48pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Multiplicator.h"

class MultiplicatorManagerUI :
	public BaseManagerShapeShifterUI<BaseManager<Multiplicator>, Multiplicator, BaseItemUI<Multiplicator>>
{
public:
	MultiplicatorManagerUI(const String &contentName);
	~MultiplicatorManagerUI();

	static MultiplicatorManagerUI * create(const String &name) { return new MultiplicatorManagerUI(name); }
};