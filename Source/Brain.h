/*
  ==============================================================================

    Brain.h
    Created: 14 Nov 2021 10:42:56pm
    Author:  No

  ==============================================================================
*/
#pragma once

#include "JuceHeader.h"
#include "Definitions/Fixture/Fixture.h"
#include "Definitions/Device/Device.h"
#include "Definitions/Group/Group.h"
#include "Definitions/Command/Command.h"
#include "Definitions/Cue/Cue.h"
#include "Definitions/Cuelist/Cuelist.h"
#include "Definitions/Preset/Preset.h"

class Brain :
    public Thread
{
public:
    juce_DeclareSingleton(Brain, true);
    Brain();
    ~Brain();

    HashMap<int, Fixture*>fixtures;
    HashMap<int, Group*>groups;
    HashMap<int, Device*>devices;
    HashMap<int, Preset*>presets;
    HashMap<int, Cuelist*>cuelists;

    Array<Cuelist*> cuelistPoolUpdating;
    Array<Cuelist*> cuelistPoolWaiting;
    Array<FixtureChannel*> fixtureChannelPoolUpdating;
    Array<FixtureChannel*> fixtureChannelPoolWaiting;

    void run() override;
    void registerFixture(Fixture* f, int id);
    void unregisterFixture(Fixture* f);
    void registerDevice(Device* f, int id);
    void unregisterDevice(Device* f);
    void registerGroup(Group* f, int id);
    void unregisterGroup(Group* f);
    void registerPreset(Preset* p, int id);
    void unregisterPreset(Preset* p);
    void registerCuelist(Cuelist* c, int id);
    void unregisterCuelist(Cuelist* c);

    void pleaseUpdate(Cuelist* c);
    void pleaseUpdate(FixtureChannel* f);

    void updateChannel(FixtureChannel* fc);

    Fixture* getFixtureById(int id);
    Device* getDeviceById(int id);
    Group* getGroupById(int id);
    Preset* getPresetById(int id);
    Cuelist* getCuelistById(int id);

    static float symPosition(int index, int nElements);
};
