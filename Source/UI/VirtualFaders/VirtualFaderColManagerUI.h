/*
  ==============================================================================

    CVVirtualFaderColManagerUI.h
    Created: 22 Feb 2018 3:42:48pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "VirtualFaderCol.h"

class VirtualFaderColManagerUI :
	public BaseManagerShapeShifterUI<BaseManager<VirtualFaderCol>, VirtualFaderCol, BaseItemUI<VirtualFaderCol>>
{
public:
	VirtualFaderColManagerUI(const String &contentName);
	~VirtualFaderColManagerUI();

	static VirtualFaderColManagerUI * create(const String &name) { return new VirtualFaderColManagerUI(name); }
};