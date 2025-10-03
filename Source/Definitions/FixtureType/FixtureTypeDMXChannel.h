/*
  ==============================================================================

    FixtureTypeDMXChannel.h
    Created: 1 Oct 2025
    Author:  j-mutter

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "FixtureTypeChannelManager.h"

class FixtureTypeDMXChannel : public BaseItem
{
public:
    FixtureTypeDMXChannel(var params = var());
    virtual ~FixtureTypeDMXChannel();

    String objectType;
    var objectData;

    IntParameter* dmxDelta;
    EnumParameter* resolution;
    
    FixtureTypeChannelManager chansManager;

    String getTypeString() const override { return objectType; }
    static FixtureTypeDMXChannel* create(var params) { return new FixtureTypeDMXChannel(params); }

    void onContainerParameterChangedInternal(Parameter* p);
};
