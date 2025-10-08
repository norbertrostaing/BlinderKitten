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
class FixtureTypeVirtualChannel;
class Fixture;
class Cuelist;
class Programmer;
class SubFixture;
class Effect;
class Carousel;
class Mapper;
class Tracker;
class SelectionMaster;
class Command;
class FixtureDMXChannel;

class SubFixtureChannel{
public:
    SubFixtureChannel();
    virtual ~SubFixtureChannel();
    bool isDeleted = false;

    bool isDirty = false;

    ChannelType* channelType = nullptr;
    String objectType = "SubFixtureChannel";
    String resolution;
    float defaultPresetValue = -1;
    float highlightValue = 0;
    float currentValue = 0.0;
    float postCuelistValue = 0;
    bool isActive = false;
    bool isHTP = false;
    bool swopKillable = false;
    bool snapOnly = false;
    bool invertOutput = false;
    bool reactToGrandMaster = false;
    int fineChannelDelta = 0;

    ChannelFamily* parentParamType = nullptr;
    ChannelType* parentParamDefinition = nullptr;
    FixtureTypeChannel* parentFixtureTypeChannel = nullptr;
    FixtureTypeVirtualChannel* parentFixtureTypeVirtualChannel = nullptr;
    Fixture* parentFixture = nullptr;
    SubFixture* parentSubFixture = nullptr;
    int subFixtureId = 0;

    FixtureDMXChannel* physicalChannel = nullptr;  // Shared DMX channel
    void writeLogicalValue(float v);               // Set logical value and notify physical channel
    bool isContributing();                         // Check if this channel is contributing to output

    SpinLock cs;
    Array<SubFixtureChannel*> virtualChildren;
    SubFixtureChannel* virtualMaster = nullptr;

    Array<Cuelist*> cuelistStack;
    Array<Programmer*> programmerStack;
    Array<Effect*> effectStack;
    Array<Carousel*> carouselStack;
    Array<Mapper*> mapperStack;
    Array<Tracker*> trackerStack;
    Array<SelectionMaster*> selectionMasterStack;
    Array<Cuelist*> cuelistFlashStack;

    Command* activeCommand = nullptr;

    void updateVal(double now);

    void cuelistOnTopOfStack(Cuelist* c);
    void cuelistOutOfStack(Cuelist* c);
    void cuelistOnTopOfFlashStack(Cuelist* c);
    void cuelistOutOfFlashStack(Cuelist* c);
    void programmerOnTopOfStack(Programmer* p);
    void programmerOutOfStack(Programmer* p);
    void effectOnTopOfStack(Effect* f);
    void effectOutOfStack(Effect* f);
    void carouselOnTopOfStack(Carousel* f);
    void carouselOutOfStack(Carousel* f);
    void mapperOnTopOfStack(Mapper* f);
    void mapperOutOfStack(Mapper* f);
    void trackerOnTopOfStack(Tracker* f);
    void trackerOutOfStack(Tracker* f);
    void selectionMasterOnTopOfStack(SelectionMaster* f);
    void selectionMasterOutOfStack(SelectionMaster* f);

};
