/*
  ==============================================================================

    StampManagerUI.h
    Created: 22 Feb 2018 3:42:48pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Stamp.h"

class StampManagerUI :
	public BaseManagerShapeShifterUI<BaseManager<Stamp>, Stamp, BaseItemUI<Stamp>>
{
public:
	StampManagerUI(const String &contentName);
	~StampManagerUI();

	static StampManagerUI * create(const String &name) { return new StampManagerUI(name); }
};