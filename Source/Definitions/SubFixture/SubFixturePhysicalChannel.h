/*
  ==============================================================================

    SubFixturePhysicalChannel.h
    Created: 24 Oct 2026 11:23:26pm
    Author:  Norb

  ==============================================================================
*/
#include <JuceHeader.h>
#include "Definitions/ChannelFamily/ChannelFamily.h"

#pragma once
class SubFixtureChannel;
class FixtureTypeChannel;
class Fixture;


class SubFixturePhysicalChannel {
public:
    SubFixturePhysicalChannel();
    virtual ~SubFixturePhysicalChannel();

    FixtureTypeChannel* parentFixtureTypeChannel = nullptr;
    Fixture* parentFixture = nullptr;

    Array<SubFixtureChannel*> writingPool;
    CriticalSection usingPool;

    bool isDirty = false;
    void setDirty();

    int fineChannelDelta = 0;

    void writeValue();
    void addSubFixtureChannelToStack(SubFixtureChannel* sfc);

};