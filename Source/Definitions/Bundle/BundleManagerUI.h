/*
  ==============================================================================

    CVGroupManagerUI.h
    Created: 22 Feb 2018 3:42:48pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Bundle.h"

class BundleManagerUI :
	public BaseManagerShapeShifterUI<BaseManager<Bundle>, Bundle, BaseItemUI<Bundle>>
{
public:
	BundleManagerUI(const String &contentName);
	~BundleManagerUI();

	static BundleManagerUI * create(const String &name) { return new BundleManagerUI(name); }
};