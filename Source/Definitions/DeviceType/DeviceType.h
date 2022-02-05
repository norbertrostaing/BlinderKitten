/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "DeviceTypeChannel.h"

class DeviceType:
    public BaseItem
{
public:
    DeviceType(var params = var());
    virtual ~DeviceType();

    String objectType;
    var objectData;
    
    Trigger* updateChannels;

    BaseManager<DeviceTypeChannel>* chansManager;
    std::unique_ptr<BaseManager<DeviceTypeChannel>> channels;
    String getTypeString() const override { return objectType; }

    void triggerTriggered(Trigger* t);
    void calcDmxChannels();
    static DeviceType* create(var params) { return new DeviceType(params); }
    void onContainerParameterChangedInternal(Parameter* p);

};

