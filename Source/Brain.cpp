/*
  ==============================================================================

    Brain.cpp
    Created: 14 Nov 2021 10:42:56pm
    Author:  No

  ==============================================================================
*/


#include "Brain.h"
#include "Definitions/ChannelValue.h"

juce_ImplementSingleton(Brain);

Brain::Brain() :
    Thread("BKBrain") 
{
    LOG("I'm a brand new brain !");
    startThread();
}

Brain :: ~Brain() {
    signalThreadShouldExit();
    waitForThreadToExit(100);
    clearSingletonInstance();
}

void Brain::run() {
    int64 now = Time::getMillisecondCounter();

    cuelistPoolUpdating.addArray(cuelistPoolWaiting);
    cuelistPoolWaiting.clear();
    for (int i = 0; i < cuelistPoolUpdating.size(); i++) {
        cuelistPoolUpdating[i]->update();
    }
    cuelistPoolUpdating.clear();

    fixtureChannelPoolUpdating.addArray(fixtureChannelPoolWaiting);
    fixtureChannelPoolWaiting.clear();
    for (int i = 0; i < fixtureChannelPoolUpdating.size(); i++) {
        fixtureChannelPoolUpdating[i] -> updateVal(now);
    }
    fixtureChannelPoolUpdating.clear();
    if (!threadShouldExit()) {
        wait(22);
        run();
    }
}

void Brain::registerFixture(Fixture* f, int id) {
    Logger::writeToLog("fixture request : " + String(id));
    int askedId = id;
    if (fixtures.containsValue(f)) {
        fixtures.removeValue(f);
    }
    bool idIsOk = false;
    while (!idIsOk) {
        if (fixtures.contains(id)) {
            id++;
        }
        else {
            idIsOk = true;
        }
    }
    fixtures.set(id, f);
    f->id->setValue(id);
    if (id != askedId) {
        Logger::writeToLog("! Fixture ID " + String(askedId) + " not available, ID " + String(id) + " given!");
    }
}

void Brain::unregisterFixture(Fixture* f) {
    if (fixtures.containsValue(f)) {
        fixtures.removeValue(f);
    }
}

void Brain::registerDevice(Device* d, int id) {
    Logger::writeToLog("device request : " + String(id));
    int askedId = id;
    if (devices.containsValue(d)) {
        devices.removeValue(d);
    }
    bool idIsOk = false;
    while (!idIsOk) {
        if (devices.contains(id)) {
            id++;
        }
        else {
            idIsOk = true;
        }
    }
    devices.set(id, d);
    d->id->setValue(id);
    if (id != askedId) {
        Logger::writeToLog("! Device ID " + String(askedId) + " not available, ID " + String(id) + " given!");
    }
}

void Brain::unregisterDevice(Device* d) {
    if (devices.containsValue(d)) {
        devices.removeValue(d);
    }
}

void Brain::registerGroup(Group* g, int id) {
    Logger::writeToLog("group request : " + String(id));
    int askedId = id;
    if (groups.containsValue(g)) {
        groups.removeValue(g);
    }
    bool idIsOk = false;
    while (!idIsOk) {
        if (groups.contains(id)) {
            id++;
        }
        else {
            idIsOk = true;
        }
    }
    groups.set(id, g);
    g->id->setValue(id);
    if (id != askedId) {
        Logger::writeToLog("! group ID " + String(askedId) + " not available, ID " + String(id) + " given!");
    }
}

void Brain::unregisterGroup(Group* g) {
    if (groups.containsValue(g)) {
        groups.removeValue(g);
    }
}

void Brain::registerPreset(Preset* p, int id) {
    Logger::writeToLog("preset request : " + String(id));
    int askedId = id;
    if (presets.containsValue(p)) {
        presets.removeValue(p);
    }
    bool idIsOk = false;
    while (!idIsOk) {
        if (presets.contains(id)) {
            id++;
        }
        else {
            idIsOk = true;
        }
    }
    presets.set(id, p);
    p->id->setValue(id);
    if (id != askedId) {
        Logger::writeToLog("! presets ID " + String(askedId) + " not available, ID " + String(id) + " given!");
    }
}

void Brain::unregisterPreset(Preset* p) {
    if (presets.containsValue(p)) {
        presets.removeValue(p);
    }
}

void Brain::registerCuelist(Cuelist* c, int id) {
    Logger::writeToLog("Cuelist request : " + String(id));
    int askedId = id;
    if (cuelists.containsValue(c)) {
        cuelists.removeValue(c);
    }
    bool idIsOk = false;
    while (!idIsOk) {
        if (cuelists.contains(id)) {
            id++;
        }
        else {
            idIsOk = true;
        }
    }
    cuelists.set(id, c);
    c->id->setValue(id);
    if (id != askedId) {
        Logger::writeToLog("! cuelists ID " + String(askedId) + " not available, ID " + String(id) + " given!");
    }
}

void Brain::unregisterCuelist(Cuelist* c) {
    if (cuelists.containsValue(c)) {
        cuelists.removeValue(c);
    }
}

void Brain::pleaseUpdate(Cuelist* c) {
    cuelistPoolWaiting.add(c);
}

void Brain::pleaseUpdate(FixtureChannel* f) {
    fixtureChannelPoolWaiting.add(f);
}

// utils

float Brain::symPosition(int index, int nElements) {
    float position = 0;
    if (nElements == 0) { return 0; }
    if (index < float(nElements / 2)) {
        position = float(index) / float((nElements - 1) / 2);
    }
    else {
        position = float(nElements - index - 1) / float((nElements - 1) / 2);
    }
    return position;
}

Fixture* Brain::getFixtureById(int id) {
    return fixtures.getReference(id);
}

Device* Brain::getDeviceById(int id) {
    return devices.getReference(id);
}

Group* Brain::getGroupById(int id) {
    return groups.getReference(id);
}

Preset* Brain::getPresetById(int id) {
    return presets.getReference(id);
}

Cuelist* Brain::getCuelistById(int id) {
    return cuelists.getReference(id);
}
