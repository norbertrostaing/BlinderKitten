/*
  ==============================================================================

    FixtureTypeLogicalChannel.h
    Created: 24 Oct 2026 11:24:00pm
    Author:  Norb

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class FixtureTypeLogicalChannel:
public BaseItem
{
    public:
    FixtureTypeLogicalChannel(var params = var());
    ~FixtureTypeLogicalChannel();

    String objectType;
    var objectData;

    Point2DParameter* dmxOutputRange;
    IntParameter* logicalPriority;

    TargetParameter* channelType;
    FloatParameter* defaultValue;
    FloatParameter* highlightValue;
    BoolParameter* killedBySWOP;
    EnumParameter* fadeOrSnap;
    TargetParameter* virtualMaster;

    Point2DParameter* physicalRange;
    Automation curve;

    void onContainerParameterChangedInternal(Parameter* p);

};