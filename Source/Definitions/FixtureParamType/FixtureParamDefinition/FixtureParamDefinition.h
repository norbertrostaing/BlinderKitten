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
    FixtureParamDefinition(var params = var());
    ~FixtureParamDefinition();

    String objectType;
    var objectData;

    EnumParameter* priority;
    BoolParameter* reactGM;
    EnumParameter* snapOrFade;
};