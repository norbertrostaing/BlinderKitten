/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
class FixtureParamType;
class FixtureParamDefinition;
class DeviceTypeChannel;
class Device;
class Cuelist;
class Programmer;
class Fixture;
class Effect;

class FixtureChannel:
    public BaseItem
{
public:
    FixtureChannel(var params = var());
    virtual ~FixtureChannel();

    String objectType;
    var objectData;

    TargetParameter* channelType;
    StringParameter* resolution;
    FloatParameter* value;

    bool isHTP = false;

    FixtureParamType* parentParamType;
    FixtureParamDefinition* parentParamDefinition;
    DeviceTypeChannel* parentDeviceTypeChannel;
    Device* parentDevice;
    Fixture* parentFixture;

    void writeValue(float v);
    String getTypeString() const override { return objectType; }
    static FixtureChannel* create(var params) { return new FixtureChannel(params); }

    Array<Cuelist*> cuelistStack;
    Array<Programmer*> programmerStack;
    Array<Effect*> effectStack;

    void updateVal(double now);

    void cuelistOnTopOfStack(Cuelist* c);
    void cuelistOutOfStack(Cuelist* c);
    void programmerOnTopOfStack(Programmer* p);
    void programmerOutOfStack(Programmer* p);
    void effectOnTopOfStack(Effect* f);
    void effectOutOfStack(Effect* f);

};
