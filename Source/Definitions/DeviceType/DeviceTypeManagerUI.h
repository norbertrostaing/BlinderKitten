/*
  ==============================================================================

    CVGroupManagerUI.h
    Created: 22 Feb 2018 3:42:48pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "DeviceType.h"

class DeviceTypeManagerUI :
	public BaseManagerShapeShifterUI<BaseManager<DeviceType>, DeviceType, BaseItemUI<DeviceType>>
{
public:
	DeviceTypeManagerUI(const String &contentName);
	~DeviceTypeManagerUI();

	static DeviceTypeManagerUI * create(const String &name) { return new DeviceTypeManagerUI(name); }
};