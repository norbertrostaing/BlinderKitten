/*
  ==============================================================================

    FixtureParamDefinition.h
    Created: 7 Nov 2021 7:40:48pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class FixtureParamDefinition:
    public BaseItem
{
    public:
    FixtureParamDefinition();
    ~FixtureParamDefinition();
    EnumParameter* priority;
    BoolParameter* reactGM;
    EnumParameter* snapOrFade;
};