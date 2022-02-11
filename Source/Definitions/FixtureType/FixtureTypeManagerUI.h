/*
  ==============================================================================

    CVGroupManagerUI.h
    Created: 22 Feb 2018 3:42:48pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "FixtureType.h"

class FixtureTypeManagerUI :
	public BaseManagerShapeShifterUI<BaseManager<FixtureType>, FixtureType, BaseItemUI<FixtureType>>
{
public:
	FixtureTypeManagerUI(const String &contentName);
	~FixtureTypeManagerUI();

	static FixtureTypeManagerUI * create(const String &name) { return new FixtureTypeManagerUI(name); }
};