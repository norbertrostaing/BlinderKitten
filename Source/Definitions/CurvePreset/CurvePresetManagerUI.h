/*
  ==============================================================================

    CVCurvePresetManagerUI.h
    Created: 22 Feb 2018 3:42:48pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "CurvePreset.h"

class CurvePresetManagerUI :
	public BaseManagerShapeShifterUI<BaseManager<CurvePreset>, CurvePreset, BaseItemUI<CurvePreset>>
{
public:
	CurvePresetManagerUI(const String &contentName);
	~CurvePresetManagerUI();

	static CurvePresetManagerUI * create(const String &name) { return new CurvePresetManagerUI(name); }
};