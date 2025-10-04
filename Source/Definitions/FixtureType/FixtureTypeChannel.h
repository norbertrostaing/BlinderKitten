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
    FloatParameter* highlightValue;
    BoolParameter* killedBySWOP;
    BoolParameter* invertOutput;
    EnumParameter* fadeOrSnap;
    Point2DParameter* dmxRange;
    
    TargetParameter* virtualMaster;

    Point2DParameter* physicalRange;
    Automation curve;

    void onContainerParameterChangedInternal(Parameter* p);
    void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) override;

    // Get parent DMX channel from object hierarchy
    class FixtureTypeDMXChannel* getParentDMXChannel();

};


