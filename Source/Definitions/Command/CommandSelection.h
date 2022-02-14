/*
  ==============================================================================

    ChannelType.h
    Created: 7 Nov 2021 7:40:48pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class CommandSelection:
    public BaseItem,
    public Inspectable::InspectableListener
{
    public:
    CommandSelection(var params = var());
    ~CommandSelection();

    String objectType;
    var objectData;

    EnumParameter* plusOrMinus;
    EnumParameter* targetType;
    IntParameter* valueFrom;
    BoolParameter* thru;
    IntParameter* valueTo;

    BoolParameter* subSel;
    IntParameter* subFrom;
    BoolParameter* subThru;
    IntParameter* subTo;

    EnumParameter* filter;
    StringParameter* pattern;
    BoolParameter* symmetry;
    IntParameter* randomSeed;
    IntParameter* randomNumber;

    void updateDisplay();

    void afterLoadJSONDataInternal();
    void onContainerParameterChangedInternal(Parameter* p);

};