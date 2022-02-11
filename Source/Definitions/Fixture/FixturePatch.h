/*
  ==============================================================================

    ChannelType.h
    Created: 7 Nov 2021 7:40:48pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class FixturePatch:
    public BaseItem
{
    public:
    FixturePatch(var params = var());
    ~FixturePatch();

    String objectType;
    var objectData;

    TargetParameter* targetInterface;
    IntParameter* address;
};