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

class SubFixture:
    public BaseItem
{
public:
    SubFixture(var params = var());
    virtual ~SubFixture();

    String objectType;
    var objectData;
    IntParameter* id;

    String suffixName = "";
    Fixture* parentFixture;

    // std::unique_ptr<BaseManager<SubFixtureChannel>> channels;
    HashMap<ChannelType*, SubFixtureChannel*> channelsMap;
    void onContainerParameterChangedInternal(Parameter* p);

    String getTypeString() const override { return objectType; }
    static SubFixture* create(var params) { return new SubFixture(params); }
};
