/*
  ==============================================================================

    ChannelType.h
    Created: 7 Nov 2021 7:40:48pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class FixtureTypeVirtualChannel:
    public BaseItem
{
    public:
    FixtureTypeVirtualChannel(var params = var());
    ~FixtureTypeVirtualChannel();

    String objectType;
    var objectData;

    TargetParameter* channelType;
    // StringParameter* suffix;
    // StringParameter* idGroupName;
    IntParameter* subFixtureId;
    FloatParameter* defaultValue;
    FloatParameter* highlightValue;
    BoolParameter* killedBySWOP;
    EnumParameter* fadeOrSnap;
    void onContainerParameterChangedInternal(Parameter* p);

};


