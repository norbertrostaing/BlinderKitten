/*
  ==============================================================================

    CVGroupManagerUI.h
    Created: 22 Feb 2018 3:42:48pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Fixture.h"

class FixtureManagerUI :
	public BaseManagerShapeShifterUI<BaseManager<Fixture>, Fixture, BaseItemUI<Fixture>>
{
public:
	FixtureManagerUI(const String &contentName);
	~FixtureManagerUI();

	static FixtureManagerUI * create(const String &name) { return new FixtureManagerUI(name); }
	void askSelectToThis(BaseItemMinimalUI<Fixture>* itemUI);
};