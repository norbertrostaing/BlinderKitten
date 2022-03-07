/*
  ==============================================================================

    Brain.h
    Created: 14 Nov 2021 10:42:56pm
    Author:  No

  ==============================================================================
*/
#pragma once

#include "JuceHeader.h"
#include "Definitions/SubFixture/SubFixture.h"
#include "Definitions/Fixture/Fixture.h"
#include "Definitions/Group/Group.h"
#include "Definitions/Command/Command.h"
#include "Definitions/Cue/Cue.h"
#include "Definitions/Cuelist/Cuelist.h"
#include "Definitions/Preset/Preset.h"
#include "Definitions/Programmer/Programmer.h"
#include "Definitions/CurvePreset/CurvePreset.h"
#include "Definitions/TimingPreset/TimingPreset.h"
#include "Definitions/Effect/Effect.h"
#include "Definitions/Carousel/Carousel.h"

class Brain :
    public Thread
{
public:
    juce_DeclareSingleton(Brain, true);
    Brain();
    ~Brain();
    void clear();

    HashMap<int, SubFixture*>SubFixtures;
    HashMap<int, Group*>groups;
    HashMap<int, Fixture*>fixtures;
    HashMap<int, Preset*>presets;
    HashMap<int, Cuelist*>cuelists;
    HashMap<int, Programmer*>programmers;
    HashMap<int, CurvePreset*>curvePresets;
    HashMap<int, TimingPreset*>timingPresets;
    HashMap<int, Effect*>effects;
    HashMap<int, Carousel*>carousels;

    OwnedArray<Cuelist> cuelistPoolUpdating;
    OwnedArray<Cuelist> cuelistPoolWaiting;
    OwnedArray<SubFixtureChannel> SubFixtureChannelPoolUpdating;
    OwnedArray<SubFixtureChannel> SubFixtureChannelPoolWaiting;
    OwnedArray<Cue> cuePoolUpdating;
    OwnedArray<Cue> cuePoolWaiting;
    OwnedArray<Programmer> programmerPoolUpdating;
    OwnedArray<Programmer> programmerPoolWaiting;
    OwnedArray<Effect> effectPoolUpdating;
    OwnedArray<Effect> effectPoolWaiting;
    OwnedArray<Carousel> carouselPoolUpdating;
    OwnedArray<Carousel> carouselPoolWaiting;

    OwnedArray<SubFixtureChannel> swoppableChannels;
    OwnedArray<Cuelist> swoppedCuelists;

    double now;
    bool updateIsRunning = false;

    void run() override;
    void registerSubFixture(SubFixture* f, int id);
    void unregisterSubFixture(SubFixture* f);
    void registerFixture(Fixture* f, int id, bool swap = false);
    void unregisterFixture(Fixture* f);
    void registerGroup(Group* f, int id, bool swap = false);
    void unregisterGroup(Group* f);
    void registerPreset(Preset* p, int id, bool swap = false);
    void unregisterPreset(Preset* p);
    void registerCuelist(Cuelist* c, int id, bool swap = false);
    void unregisterCuelist(Cuelist* c);
    void registerProgrammer(Programmer* p, int id, bool swap = false);
    void unregisterProgrammer(Programmer* p);
    void registerCurvePreset(CurvePreset* p, int id, bool swap = false);
    void unregisterCurvePreset(CurvePreset* p);
    void registerTimingPreset(TimingPreset* p, int id, bool swap = false);
    void unregisterTimingPreset(TimingPreset* p);
    void registerEffect(Effect* p, int id, bool swap = false);
    void unregisterEffect(Effect* p);
    void registerCarousel(Carousel* p, int id, bool swap = false);
    void unregisterCarousel(Carousel* p);

    void pleaseUpdate(Cuelist* c);
    void pleaseUpdate(SubFixtureChannel* f);
    void pleaseUpdate(Cue* c);
    void pleaseUpdate(Programmer* p);
    void pleaseUpdate(Effect* f);
    void pleaseUpdate(Carousel* c);

    //void updateChannel(SubFixtureChannel* fc);

    SubFixture* getSubFixtureById(int id);
    Fixture* getFixtureById(int id);
    Group* getGroupById(int id);
    Preset* getPresetById(int id);
    Cuelist* getCuelistById(int id);
    Programmer* getProgrammerById(int id);
    CurvePreset* getCurvePresetById(int id);
    TimingPreset* getTimingPresetById(int id);
    Effect* getEffectById(int id);
    Carousel* getCarouselById(int id);

    void swoppedCuelist(Cuelist* c);
    void unswoppedCuelist(Cuelist* c);
    bool isSwopping = false;

    static float symPosition(int index, int nElements);
};
