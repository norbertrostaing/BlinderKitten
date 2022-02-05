/*
  ==============================================================================

    CVGroupManagerUI.h
    Created: 22 Feb 2018 3:42:48pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "FixtureParamType.h"

class FixtureParamTypeManagerUI :
	public BaseManagerShapeShifterUI<BaseManager<FixtureParamType>, FixtureParamType, BaseItemUI<FixtureParamType>>
{
public:
	FixtureParamTypeManagerUI(const String &contentName);
	~FixtureParamTypeManagerUI();

	static FixtureParamTypeManagerUI * create(const String &name) { return new FixtureParamTypeManagerUI(name); }
};