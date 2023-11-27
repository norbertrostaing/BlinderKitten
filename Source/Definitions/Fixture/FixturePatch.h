/*
  ==============================================================================

    ChannelType.h
    Created: 7 Nov 2021 7:40:48pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "FixturePatchCorrection.h"

class Fixture;

class FixturePatch:
    public BaseItem
{
    public:
    FixturePatch(var params = var());
    ~FixturePatch();

    String objectType;
    var objectData;

    Array<int> currentAdresses;
    TargetParameter* currentInterface;

    TargetParameter* targetInterface;
    IntParameter* address;
    BaseManager<FixturePatchCorrection> corrections;

    bool patchingInProgess = false;

    void onContainerParameterChangedInternal(Parameter* p);
    void tryToEnablePatch();
    void disableCurrentPatch();
    int getFixtureId();

};