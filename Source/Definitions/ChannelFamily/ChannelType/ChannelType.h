/*
  ==============================================================================

    ChannelType.h
    Created: 7 Nov 2021 7:40:48pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class ChannelType:
    public BaseItem
{
    public:
    ChannelType(var params = var());
    ~ChannelType();

    String objectType;
    var objectData;

    EnumParameter* priority;
    BoolParameter* reactGM;
    EnumParameter* snapOrFade;
};