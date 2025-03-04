/*
  ==============================================================================

    CVBKPathPresetManagerUI.h
    Created: 22 Feb 2018 3:42:48pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "BKPathPreset.h"

class BKPathPresetManagerUI :
	public BaseManagerShapeShifterUI<BaseManager<BKPathPreset>, BKPathPreset, BaseItemUI<BKPathPreset>>
{
public:
	BKPathPresetManagerUI(const String &contentName);
	~BKPathPresetManagerUI();

	static BKPathPresetManagerUI * create(const String &name) { return new BKPathPresetManagerUI(name); }
};