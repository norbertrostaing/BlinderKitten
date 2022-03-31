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
    void clearUpdates();

    HashMap<int, SubFixture*>subFixtures;
    HashMap<int, Group*>groups;
    HashMap<int, Fixture*>fixtures;
    HashMap<int, Preset*>presets;
    HashMap<int, Cuelist*>cuelists;
    HashMap<int, Programmer*>programmers;
    HashMap<int, CurvePreset*>curvePresets;
    HashMap<int, TimingPreset*>timingPresets;
    HashMap<int, Effect*>effects;
    HashMap<int, Carousel*>carousels;

    std::vector<Cuelist*> cuelistPoolUpdating;
    std::vector<Cuelist*> cuelistPoolWaiting;
    std::vector<SubFixtureChannel*> subFixtureChannelPoolUpdating;
    std::vector<SubFixtureChannel*> subFixtureChannelPoolWaiting;
    std::vector<Cue*> cuePoolUpdating;
    std::vector<Cue*> cuePoolWaiting;
    std::vector<Programmer*> programmerPoolUpdating;
    std::vector<Programmer*> programmerPoolWaiting;
    std::vector<Effect*> effectPoolUpdating;
    std::vector<Effect*> effectPoolWaiting;
    std::vector<Carousel*> carouselPoolUpdating;
    std::vector<Carousel*> carouselPoolWaiting;

    Array<SubFixtureChannel*> swoppableChannels;
    Array<Cuelist*> swoppedCuelists;

    double now;
    bool updateCuesIsRunning = false;
    bool updateCuelistsIsRunning = false;
    bool updateCarouselsIsRunning = false;
    bool updateEffectsIsRunning = false;
    bool updateProgrammersIsRunning = false;
    bool updateChannelsIsRunning = false;
    bool loadingIsRunning = false;

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
