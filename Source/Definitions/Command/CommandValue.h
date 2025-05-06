/*
  ==============================================================================

    ChannelType.h
    Created: 7 Nov 2021 7:40:48pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class CommandValue:
    public BaseItem,
    public Inspectable::InspectableListener
{
    public:
    CommandValue(var params = var());
    ~CommandValue();

    String objectType;
    var objectData;

    EnumParameter* presetOrValue;

    TargetParameter* channelType;

    BoolParameter* release;

    IntParameter* presetIdFrom;
    FloatParameter* valueFrom;

    BoolParameter* thru;

    IntParameter* presetIdTo;
    FloatParameter* valueTo;
    BoolParameter* symmetry;
    BoolParameter* randomize;

    BoolParameter* HTPOverride;

    bool shouldShowStepSize = false;
    FloatParameter* stepSize;

    void updateDisplay();

    void afterLoadJSONDataInternal();
    void onContainerParameterChangedInternal(Parameter* p);

};