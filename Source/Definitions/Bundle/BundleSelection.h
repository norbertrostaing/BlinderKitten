/*
  ==============================================================================

    ChannelType.h
    Created: 7 Nov 2021 7:40:48pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Definitions/SubFixture/SubFixture.h"

class BundleSelection:
    public BaseItem,
    public Inspectable::InspectableListener
{
    public:
    BundleSelection(var params = var());
    ~BundleSelection();

    String objectType;
    var objectData;

    EnumParameter* plusOrMinus;
    EnumParameter* targetType;
    IntParameter* valueFrom;
    BoolParameter* thru;
    IntParameter* valueTo;

    void updateDisplay();

    void afterLoadJSONDataInternal();
    void onContainerParameterChangedInternal(Parameter* p);

};