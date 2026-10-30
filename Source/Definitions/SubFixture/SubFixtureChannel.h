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
class ChannelValue;
class Stamp;
class SubFixturePhysicalChannel;
class FixtureTypeLogicalChannel;

class SubFixtureChannel{
public:
    SubFixtureChannel();
    virtual ~SubFixtureChannel();
    bool isDeleted = false;

    bool isDirty = false;
    bool forceNextWrite = false;

    bool isActive = false;

    double lowPhysicalValue = 0;
    double highPhysicalValue = 1;

    SubFixturePhysicalChannel* physicalChannel = nullptr;
    ChannelType* channelType = nullptr;
    String objectType = "SubFixtureChannel";
    String resolution;
    bool userDefinedDefaultValue = false;
    float defaultValue = -1;
    float defaultPresetValue = -1;
    float highlightValue = 0;
    float currentValue = -1;
    float postCuelistValue = 0;
    bool isHTP = false;
    bool swopKillable = false;
    bool snapOnly = false;
    bool invertOutput = false;
    bool reactToGrandMaster = false;
    int logicalPriority = 0;

    Automation* outputCurve = nullptr;

    FixtureTypeChannel* parentFixtureTypeChannel = nullptr;
    FixtureTypeLogicalChannel* parentFixtureTypeLogicalChannel = nullptr;
    FixtureTypeVirtualChannel* parentFixtureTypeVirtualChannel = nullptr;
    Fixture* parentFixture = nullptr;
    SubFixture* parentSubFixture = nullptr;
    int subFixtureId = 0;

    void writeValue(float v);

    Array<std::shared_ptr<ChannelValue>> liveCV;

    CriticalSection cs;
    Array<SubFixtureChannel*> virtualChildren;
    SubFixtureChannel* virtualMaster = nullptr;

    Array<Cuelist*> cuelistStack;
    Array<Programmer*> programmerStack;
    Array<Effect*> effectStack;
    Array<Stamp*> stampStack;
    Array<Carousel*> carouselStack;
    Array<Mapper*> mapperStack;
    Array<Tracker*> trackerStack;
    Array<SelectionMaster*> selectionMasterStack;
    Array<Cuelist*> cuelistFlashStack;

    Array<Cuelist*> cuelistStackSnapshot;
    Array<Programmer*> programmerStackSnapshot;
    Array<Effect*> effectStackSnapshot;
    Array<Stamp*> stampStackSnapshot;
    Array<Carousel*> carouselStackSnapshot;
    Array<Mapper*> mapperStackSnapshot;
    Array<Tracker*> trackerStackSnapshot;
    Array<SelectionMaster*> selectionMasterStackSnapshot;
    Array<Cuelist*> cuelistFlashStackSnapshot;

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
    void stampOnTopOfStack(Stamp* f);
    void stampOutOfStack(Stamp* f);
    void carouselOnTopOfStack(Carousel* f);
    void carouselOutOfStack(Carousel* f);
    void mapperOnTopOfStack(Mapper* f);
    void mapperOutOfStack(Mapper* f);
    void trackerOnTopOfStack(Tracker* f);
    void trackerOutOfStack(Tracker* f);
    void selectionMasterOnTopOfStack(SelectionMaster* f);
    void selectionMasterOutOfStack(SelectionMaster* f);

};
