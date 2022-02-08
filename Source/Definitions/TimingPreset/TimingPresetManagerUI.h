/*
  ==============================================================================

    CVTimingPresetManagerUI.h
    Created: 22 Feb 2018 3:42:48pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "TimingPreset.h"

class TimingPresetManagerUI :
	public BaseManagerShapeShifterUI<BaseManager<TimingPreset>, TimingPreset, BaseItemUI<TimingPreset>>
{
public:
	TimingPresetManagerUI(const String &contentName);
	~TimingPresetManagerUI();

	static TimingPresetManagerUI * create(const String &name) { return new TimingPresetManagerUI(name); }
};