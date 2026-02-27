/*
  ==============================================================================

    Brain.h
    Created: 14 Nov 2021 10:42:56pm
    Author:  No

  ==============================================================================
*/
#pragma once

#include "JuceHeader.h"

class SubFixture;
class SubFixtureChannel;
class FixtureType;
class Fixture;
class Group;
class Command;
class CommandSelection;
class CommandValue;
class Cue;
class Cuelist;
class Preset;
class Programmer;
class CurvePreset;
class TimingPreset;
class BKPathPreset;
class Effect;
class Stamp;
class Carousel;
class Mapper;
class RunningTask;
class Task;
class Layout;
class Tracker;
class SelectionMaster;
class Bundle;
class DMXArtNetDevice;


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
    HashMap<int, BKPathPreset*>bkPathPresets;
    HashMap<int, Effect*>effects;
    HashMap<int, Stamp*>stamps;
    HashMap<int, Carousel*>carousels;
    HashMap<int, Mapper*>mappers;
    HashMap<int, Tracker*>trackers;
    HashMap<int, SelectionMaster*>selectionMasters;
    HashMap<int, Layout*>layouts;
    HashMap<int, Bundle*>bundles;

    Array<Cuelist*> cuelistPoolUpdating;
    Array<Cuelist*> cuelistPoolWaiting;
    Array<SubFixtureChannel*> subFixtureChannelPoolUpdating;
    Array<SubFixtureChannel*> subFixtureChannelPoolWaiting;
    Array<Cue*> cuePoolUpdating;
    Array<Cue*> cuePoolWaiting;
    Array<Programmer*> programmerPoolUpdating;
    Array<Programmer*> programmerPoolWaiting;
    Array<Effect*> effectPoolUpdating;
    Array<Effect*> effectPoolWaiting;
    Array<Stamp*> stampPoolUpdating;
    Array<Stamp*> stampPoolWaiting;
    Array<Carousel*> carouselPoolUpdating;
    Array<Carousel*> carouselPoolWaiting;
    Array<Mapper*> mapperPoolUpdating;
    Array<Mapper*> mapperPoolWaiting;
    Array<Tracker*> trackerPoolUpdating;
    Array<Tracker*> trackerPoolWaiting;
    Array<SelectionMaster*> selectionMasterPoolUpdating;
    Array<SelectionMaster*> selectionMasterPoolWaiting;

    Array<SubFixtureChannel*> swoppableChannels;
    Array<SubFixtureChannel*> grandMasterChannels;
    Array<Cuelist*> swoppedCuelists;
    Array<Effect*> swoppedEffects;
    Array<Stamp*> swoppedStamps;
    Array<Carousel*> swoppedCarousels;

    Array<Command*> allCommands;
    Array<CommandSelection*> allCommandSelections;
    Array<CommandValue*> allCommandValues;
    Array<SubFixture*> allSubfixtures;
    Array<SubFixtureChannel*> allSubfixtureChannels;

    Array<DMXArtNetDevice*> syncedArtnetDevices;

    Random mainRandom;

    double now;
    bool loadingIsRunning = false;
    bool isClearing = false;

    bool reconstructVirtuals = false;
    bool virtualFadersNeedUpdate = false;
    bool virtualFaderButtonsNeedUpdate = false;
    bool virtualButtonsNeedUpdate = false;
    bool encoderValuesNeedRefresh = false;
    bool cuelistGridNeedRefresh = false;
    bool carouselGridNeedRefresh = false;
    bool effectGridNeedRefresh = false;
    bool mapperGridNeedRefresh = false;
    bool layoutViewerNeedRefresh = false;
    bool layoutViewerNeedRepaint = false;
    bool defaultValuesNeedRefresh = false;

    void run() override;
    void brainLoop();
    bool skipLoop = false;

    double TSBundles = 0;
    CriticalSection usingCollections;
    CriticalSection usingTasksCollection;
    void registerSubFixture(SubFixture* f, int id);
    void unregisterSubFixture(SubFixture* f);
    void registerFixture(Fixture* f, int id);
    void unregisterFixture(Fixture* f);
    void registerGroup(Group* f, int id);
    void unregisterGroup(Group* f);
    void registerPreset(Preset* p, int id);
    void unregisterPreset(Preset* p);
    void registerCuelist(Cuelist* c, int id, bool swap = false);
    void unregisterCuelist(Cuelist* c);
    void registerProgrammer(Programmer* p, int id, bool swap = false);
    void unregisterProgrammer(Programmer* p);
    void registerCurvePreset(CurvePreset* p, int id, bool swap = false);
    void unregisterCurvePreset(CurvePreset* p);
    void registerTimingPreset(TimingPreset* p, int id, bool swap = false);
    void unregisterTimingPreset(TimingPreset* p);
    void registerBKPathPreset(BKPathPreset* p, int id, bool swap = false);
    void unregisterBKPathPreset(BKPathPreset* p);
    void registerEffect(Effect* p, int id, bool swap = false);
    void unregisterEffect(Effect* p);
    void registerStamp(Stamp* p, int id, bool swap = false);
    void unregisterStamp(Stamp* p);
    void registerCarousel(Carousel* p, int id, bool swap = false);
    void unregisterCarousel(Carousel* p);
    void registerMapper(Mapper* p, int id, bool swap = false);
    void unregisterMapper(Mapper* p);
    void registerLayout(Layout* p, int id, bool swap = false);
    void unregisterLayout(Layout* p);
    void registerTracker(Tracker* p, int id, bool swap = false);
    void unregisterTracker(Tracker* p);
    void registerSelectionMaster(SelectionMaster* p, int id, bool swap = false);
    void unregisterSelectionMaster(SelectionMaster* p);
    void registerBundle(Bundle* p, int id, bool swap = false);
    void unregisterBundle(Bundle* p);

    void pleaseUpdate(Cuelist* c);
    void pleaseUpdate(SubFixtureChannel* f);
    void pleaseUpdate(Cue* c);
    void pleaseUpdate(Programmer* p);
    void pleaseUpdate(Effect* f);
    void pleaseUpdate(Stamp* s);
    void pleaseUpdate(Carousel* c);
    void pleaseUpdate(Mapper* c);
    void pleaseUpdate(Tracker* c);
    void pleaseUpdate(SelectionMaster* c);

    void updateAllChannels();

    void grandMasterChanged();

    //void updateChannel(SubFixtureChannel* fc);

    SubFixture* getSubFixtureById(int id);
    Fixture* getFixtureById(int id, bool followIfAnother = false);
    Group* getGroupById(int id, bool followIfAnother = false);
    Preset* getPresetById(int id, bool followIfAnother = false);
    Cuelist* getCuelistById(int id);
    Programmer* getProgrammerById(int id);
    CurvePreset* getCurvePresetById(int id);
    TimingPreset* getTimingPresetById(int id, bool followIfAnother = false);
    BKPathPreset* getBKPathPresetById(int id, bool followIfAnother = false);
    Effect* getEffectById(int id);
    Stamp* getStampById(int id);
    Carousel* getCarouselById(int id);
    Mapper* getMapperById(int id);
    Layout* getLayoutById(int id);
    Tracker* getTrackerById(int id);
    SelectionMaster* getSelectionMasterById(int id);
    Bundle* getBundleById(int id);

    void swoppedCuelist(Cuelist* c);
    void swoppedEffect(Effect* c);
    void swoppedStamp(Stamp* c);
    void swoppedCarousel(Carousel* c);
    void unswoppedCuelist(Cuelist* c);
    void unswoppedEffect(Effect* c);
    void unswoppedStamp(Stamp* c);
    void unswoppedCarousel(Carousel* c);
    bool isSwopping = false;

    static float symPosition(int index, int nElements);

    OwnedArray<RunningTask> runningTasks;
    int currentTaskId = 0;
    int newTaskId();
    void startTask(Task* t, double startTime, int cuelistId, float forcedDelay = -1, float forceFade = -1);
    void stopTasks(int cuelistId, int taskId);

    void goAllLoadedCuelists();
    void killAllCuelists();
    void offAllCuelists();
    void stopAllEffects();
    void stopAllStamps();
    void stopAllCarousels();

    void resetRandomSeed(int seed);

    void showWindow(String name);

    void loadRunningCuelistsInProgrammer();
    void loadRunningCuelistsInProgrammer(Programmer* p);

    void soloPoolCuelistStarted(int poolId, Cuelist* c);
    void soloPoolEffectStarted(int poolId, Effect* c);
    void soloPoolStampStarted(int poolId, Stamp* c);
    void soloPoolCarouselStarted(int poolId, Carousel* c);
    void soloPoolCheck(int poolId, String excludeType, int excludeId);
    void soloPoolRandom(int poolId);
    void soloPoolStop(int poolId);

    void replaceFixtureIdEverywhere(int from, int to);
    void replaceGroupIdEverywhere(int from, int to);
    void replacePresetIdEverywhere(int from, int to);



};
