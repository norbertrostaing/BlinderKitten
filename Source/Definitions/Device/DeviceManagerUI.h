/*
  ==============================================================================

    CVGroupManagerUI.h
    Created: 22 Feb 2018 3:42:48pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Device.h"

class DeviceManagerUI :
	public BaseManagerShapeShifterUI<BaseManager<Device>, Device, BaseItemUI<Device>>
{
public:
	DeviceManagerUI(const String &contentName);
	~DeviceManagerUI();

	static DeviceManagerUI * create(const String &name) { return new DeviceManagerUI(name); }
};