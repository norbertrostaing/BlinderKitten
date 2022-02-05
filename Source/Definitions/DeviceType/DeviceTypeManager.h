/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "DeviceType.h"

class DeviceTypeManager :
    public BaseManager<DeviceType>
{
public:
    juce_DeclareSingleton(DeviceTypeManager, true);

    DeviceTypeManager();
    ~DeviceTypeManager();

    void addItemInternal(DeviceType* o, var data) override;
    void removeItemInternal(DeviceType* o) override;

    void onContainerParameterChanged(Parameter* p) override;

};