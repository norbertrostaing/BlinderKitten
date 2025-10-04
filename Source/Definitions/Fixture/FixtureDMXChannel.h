/*
  ==============================================================================

    FixtureDMXChannel.h
    Created: 1 Oct 2025
    Author:  j-mutter

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class FixtureTypeDMXChannel;
class SubFixtureChannel;
class Fixture;

class FixtureDMXChannel
{
public:
    FixtureDMXChannel(Fixture* parentFixture, FixtureTypeDMXChannel* typeChannel);
    virtual ~FixtureDMXChannel();

    Fixture* parentFixture;
    FixtureTypeDMXChannel* fixtureTypeDMXChannel;

    Array<SubFixtureChannel*> logicalChannels;
    SubFixtureChannel* activeLogicalChannel;

    // Methods
    void registerLogicalChannel(SubFixtureChannel* sfc);
    void unregisterLogicalChannel(SubFixtureChannel* sfc);
    void onLogicalChannelChanged(SubFixtureChannel* sfc);
    void resolveAndOutput();

private:
    void resolveActiveLogicalChannel();
    void outputToHardware();

    float activeDMXRangeMin();
    float activeDMXRangeMax();

    SpinLock outputLock;
};
