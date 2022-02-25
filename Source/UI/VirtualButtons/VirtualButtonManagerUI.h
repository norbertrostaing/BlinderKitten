/*
  ==============================================================================

    CVVirtualButtonManagerUI.h
    Created: 22 Feb 2018 3:42:48pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "VirtualButton.h"

class VirtualButtonManagerUI :
	public BaseManagerShapeShifterUI<BaseManager<VirtualButton>, VirtualButton, BaseItemUI<VirtualButton>>
{
public:
	VirtualButtonManagerUI(const String &contentName);
	~VirtualButtonManagerUI();

	static VirtualButtonManagerUI * create(const String &name) { return new VirtualButtonManagerUI(name); }
};