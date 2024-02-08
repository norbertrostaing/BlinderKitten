/*
  ==============================================================================

    TrackerManagerUI.h
    Created: 22 Feb 2018 3:42:48pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Tracker.h"

class TrackerManagerUI :
	public BaseManagerShapeShifterUI<BaseManager<Tracker>, Tracker, BaseItemUI<Tracker>>
{
public:
	TrackerManagerUI(const String &contentName);
	~TrackerManagerUI();

	static TrackerManagerUI * create(const String &name) { return new TrackerManagerUI(name); }
};