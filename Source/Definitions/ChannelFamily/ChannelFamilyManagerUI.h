/*
  ==============================================================================

    CVGroupManagerUI.h
    Created: 22 Feb 2018 3:42:48pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "ChannelFamily.h"

class ChannelFamilyManagerUI :
	public BaseManagerShapeShifterUI<BaseManager<ChannelFamily>, ChannelFamily, BaseItemUI<ChannelFamily>>
{
public:
	ChannelFamilyManagerUI(const String &contentName);
	~ChannelFamilyManagerUI();

	static ChannelFamilyManagerUI * create(const String &name) { return new ChannelFamilyManagerUI(name); }
};