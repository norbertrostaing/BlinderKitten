/*
  ==============================================================================

    CVGroupManagerUI.h
    Created: 22 Feb 2018 3:42:48pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "SubFixture.h"

class SubFixtureManagerUI :
	public BaseManagerShapeShifterUI<BaseManager<SubFixture>, SubFixture, BaseItemUI<SubFixture>>
{
public:
	SubFixtureManagerUI(const String &contentName);
	~SubFixtureManagerUI();

	static SubFixtureManagerUI * create(const String &name) { return new SubFixtureManagerUI(name); }
};