/*
  ==============================================================================

    FixtureParamDefinition.h
    Created: 7 Nov 2021 7:40:48pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class DeviceTypeChannel:
    public BaseItem
{
    public:
    DeviceTypeChannel(var params = var());
    ~DeviceTypeChannel();

    String objectType;
    var objectData;

    TargetParameter* channelType;
    StringParameter* suffix;
    StringParameter* idGroupName;
    FloatParameter* defaultValue;
    EnumParameter* resolution;
    IntParameter* dmxDelta;
};


