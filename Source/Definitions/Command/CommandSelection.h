/*
  ==============================================================================

    FixtureParamDefinition.h
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
    CommandSelection();
    ~CommandSelection();

    EnumParameter* plusOrMinus;
    EnumParameter* targetType;
    IntParameter* valueFrom;
    BoolParameter* thru;
    IntParameter* valueTo;

    EnumParameter* filter;
    StringParameter* pattern;
    BoolParameter* symmetry;

    void updateDisplay();

    void afterLoadJSONDataInternal();
    void onContainerParameterChangedInternal(Parameter* p);

};