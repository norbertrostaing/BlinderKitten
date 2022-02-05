/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "FixtureChannel.h"
#include "Definitions/FixtureParamType/FixtureParamDefinition/FixtureParamDefinition.h"

class Fixture:
    public BaseItem
{
public:
    Fixture(var params = var());
    virtual ~Fixture();

    String objectType;
    var objectData;
    IntParameter* id;

    String suffixName = "";
    Device* parentDevice;

    std::unique_ptr<BaseManager<FixtureChannel>> channels;
    HashMap<FixtureParamDefinition*, FixtureChannel*> channelsMap;
    void onContainerParameterChangedInternal(Parameter* p);

    String getTypeString() const override { return objectType; }
    static Fixture* create(var params) { return new Fixture(params); }
};
