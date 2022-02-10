/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "DevicePatch.h"
#include "../Fixture/Fixture.h"

class Device:
    public BaseItem
{
public:
    Device(var params = var());
    virtual ~Device();

    void onContainerNiceNameChanged() override;
    void afterLoadJSONDataInternal() override;

    String objectType;
    var objectData;

    IntParameter* id;
    StringParameter* userName;
    void onContainerParameterChangedInternal(Parameter* p);
    void updateName();
    TargetParameter* devTypeParam;

    BaseManager<DevicePatch> patchs;
    BaseManager<Fixture> fixtures;
    String getTypeString() const override { return objectType; }
    Array<FixtureChannel*> channels;

    void applyPatchs();
    void checkChildrenFixtures();

    static Device* create(var params) { return new Device(params); }
};
