/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "Device.h"

class DeviceManager :
    public BaseManager<Device>
{
public:
    juce_DeclareSingleton(DeviceManager, true);

    DeviceManager();
    ~DeviceManager();

    void addItemInternal(Device* o, var data) override;
    void removeItemInternal(Device* o) override;

};