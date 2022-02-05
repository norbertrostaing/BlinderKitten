/*
  ==============================================================================

    CVGroupManagerUI.h
    Created: 22 Feb 2018 3:42:48pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Preset.h"

class PresetManagerUI :
	public BaseManagerShapeShifterUI<BaseManager<Preset>, Preset, BaseItemUI<Preset>>
{
public:
	PresetManagerUI(const String &contentName);
	~PresetManagerUI();

	static PresetManagerUI * create(const String &name) { return new PresetManagerUI(name); }
};