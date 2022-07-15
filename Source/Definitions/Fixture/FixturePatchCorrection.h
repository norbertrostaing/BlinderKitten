/*
  ==============================================================================

    ChannelType.h
    Created: 7 Nov 2021 7:40:48pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class FixturePatchCorrection:
    public BaseItem
{
    public:
    FixturePatchCorrection(var params = var());
    ~FixturePatchCorrection();

    var objectData;
    String objectType;

    TargetParameter* channelType;
    BoolParameter* invertChannel;
    FloatParameter* offsetValue;
    Automation curve;


};