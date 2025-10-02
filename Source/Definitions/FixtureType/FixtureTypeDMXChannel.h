/*
  ==============================================================================

    FixtureTypeDMXChannel.h
    Created: 1 Oct 2025
    Author:  j-mutter

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class FixtureTypeDMXChannel : public BaseItem
{
public:
    FixtureTypeDMXChannel(var params = var());
    virtual ~FixtureTypeDMXChannel();

    String objectType;
    var objectData;

    IntParameter* dmxDelta;
    FloatParameter* highlightValue;
    EnumParameter* resolution;

    String getTypeString() const override { return objectType; }
    static FixtureTypeDMXChannel* create(var params) { return new FixtureTypeDMXChannel(params); }

    void onContainerParameterChangedInternal(Parameter* p);
};
