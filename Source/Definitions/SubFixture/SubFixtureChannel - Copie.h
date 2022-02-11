/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
class ChannelFamily;
class ChannelType;
class FixtureTypeChannel;
class Fixture;
class Cuelist;
class Programmer;
class SubFixture;
class Effect;

class SubFixtureChannel:
    public BaseItem
{
public:
    SubFixtureChannel(var params = var());
    virtual ~SubFixtureChannel();

    String objectType;
    var objectData;

    TargetParameter* channelType;
    StringParameter* resolution;
    FloatParameter* value;

    bool isHTP = false;

    ChannelFamily* parentParamType;
    ChannelType* parentParamDefinition;
    FixtureTypeChannel* parentFixtureTypeChannel;
    Fixture* parentFixture;
    SubFixture* parentSubFixture;

    void writeValue(float v);
    String getTypeString() const override { return objectType; }
    static SubFixtureChannel* create(var params) { return new SubFixtureChannel(params); }

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
