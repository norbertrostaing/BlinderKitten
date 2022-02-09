/*
  ==============================================================================

    FixtureParamDefinition.h
    Created: 7 Nov 2021 7:40:48pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class DevicePatch:
    public BaseItem
{
    public:
    DevicePatch();
    ~DevicePatch();

    TargetParameter* targetInterface;
    IntParameter* address;
};