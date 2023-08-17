/*
  ==============================================================================

    CVGroupManagerUI.h
    Created: 22 Feb 2018 3:42:48pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Layout.h"

class LayoutManagerUI :
	public BaseManagerShapeShifterUI<BaseManager<Layout>, Layout, BaseItemUI<Layout>>
{
public:
	LayoutManagerUI(const String &contentName);
	~LayoutManagerUI();

	static LayoutManagerUI * create(const String &name) { return new LayoutManagerUI(name); }
};