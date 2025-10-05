/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "SubFixtureChannel.h"
#include "Definitions/ChannelFamily/ChannelType/ChannelType.h"

class SubFixture {
public:
    SubFixture();
    virtual ~SubFixture();

    struct MyHashGenerator:juce::DefaultHashFunctions
    {
        int generateHash(SubFixture* key, int upperLimit) const;
    };

    Fixture* parentFixture;
    int subId;
    String displayName = "";

    // Highlight state (set by Programmer)
    bool isHighlighted = false;

    OwnedArray<SubFixtureChannel> channelsContainer;
    HashMap<ChannelType*, SubFixtureChannel*> channelsMap;
    Colour getOutputColor();
};
