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

    Fixture* parentFixture;
    int subId;
    HashMap<ChannelType*, SubFixtureChannel*> channelsMap;

};
