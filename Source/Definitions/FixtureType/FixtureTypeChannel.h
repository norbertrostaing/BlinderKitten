/*
  ==============================================================================

    ChannelType.h
    Created: 7 Nov 2021 7:40:48pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class FixtureTypeChannel:
    public BaseItem
{
    public:
    FixtureTypeChannel(var params = var());
    ~FixtureTypeChannel();

    String objectType;
    var objectData;

    TargetParameter* channelType;
    // StringParameter* suffix;
    // StringParameter* idGroupName;
    IntParameter* subFixtureId;
    FloatParameter* defaultValue;
    BoolParameter* killedBySWOP;
    EnumParameter* resolution;
    EnumParameter* fadeOrSnap;
    IntParameter* dmxDelta;
    void onContainerParameterChangedInternal(Parameter* p);

};


