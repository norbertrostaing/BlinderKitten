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
    startThread();
}

Brain :: ~Brain() {
    signalThreadShouldExit();
    waitForThreadToExit(100);
    clearSingletonInstance();
}

void Brain::run() {
    while(!threadShouldExit()) {
        now = Time::getMillisecondCounterHiRes();
        ///double now = Time::getApproximateMillisecondCounter();
        cuePoolUpdating.addArray(cuePoolWaiting);
        cuePoolWaiting.clear();
        for (int i = 0; i < cuePoolUpdating.size(); i++) {
            cuePoolUpdating[i]->update(now);
        }
        cuePoolUpdating.clear();

        cuelistPoolUpdating.addArray(cuelistPoolWaiting);
        cuelistPoolWaiting.clear();
        for (int i = 0; i < cuelistPoolUpdating.size(); i++) {
            cuelistPoolUpdating[i]->update();
        }
        cuelistPoolUpdating.clear();

        effectPoolUpdating.addArray(effectPoolWaiting);
        effectPoolWaiting.clear();
        for (int i = 0; i < effectPoolUpdating.size(); i++) {
            effectPoolUpdating[i]->update(now);
        }
        effectPoolUpdating.clear();

        carouselPoolUpdating.addArray(carouselPoolWaiting);
        carouselPoolWaiting.clear();
        for (int i = 0; i < carouselPoolUpdating.size(); i++) {
            carouselPoolUpdating[i]->update(now);
        }
        carouselPoolUpdating.clear();

        programmerPoolUpdating.addArray(programmerPoolWaiting);
        programmerPoolWaiting.clear();
        for (int i = 0; i < programmerPoolUpdating.size(); i++) {
            programmerPoolUpdating[i]->update(now);
        }
        programmerPoolUpdating.clear();

        SubFixtureChannelPoolUpdating.addArray(SubFixtureChannelPoolWaiting);
        SubFixtureChannelPoolWaiting.clear();
        for (int i = 0; i < SubFixtureChannelPoolUpdating.size(); i++) {
            SubFixtureChannelPoolUpdating[i] -> updateVal(now);
        }
        SubFixtureChannelPoolUpdating.clear();
        wait(22);
    }
}



void Brain::registerSubFixture(SubFixture* f, int id) {
    int askedId = id;
    if (SubFixtures.getReference(id) == f) { return; }
    if (SubFixtures.containsValue(f)) {
        SubFixtures.removeValue(f);
    }
    bool idIsOk = false;
    while (!idIsOk) {
        if (SubFixtures.contains(id) && SubFixtures.getReference(id) != nullptr) {
            id++;
        }
        else {
            idIsOk = true;
        }
    }
    SubFixtures.set(id, f);
    f->subId =id;
    if (id != askedId) {
    }
}

void Brain::unregisterSubFixture(SubFixture* f) {
    if (SubFixtures.containsValue(f)) {
        SubFixtures.removeValue(f);
    }
}

void Brain::registerFixture(Fixture* d, int id) {
    int askedId = id;
    if (Fixtures.getReference(id) == d) { return; }
    if (Fixtures.containsValue(d)) {
        Fixtures.removeValue(d);
    }
    bool idIsOk = false;
    while (!idIsOk) {
        if (Fixtures.contains(id) && Fixtures.getReference(id) != nullptr) {
            id++;
        }
        else {
            idIsOk = true;
        }
    }
    Fixtures.set(id, d);
    d->id->setValue(id);
    if (id != askedId) {
    }
}

void Brain::unregisterFixture(Fixture* d) {
    if (Fixtures.containsValue(d)) {
        Fixtures.removeValue(d);
    }
}

void Brain::registerGroup(Group* g, int id) {
    int askedId = id;
    if (groups.getReference(id) == g) {return;}
    if (groups.containsValue(g)) {
        groups.removeValue(g);
    }
    bool idIsOk = false;
    while (!idIsOk) {
        if (groups.contains(id) && groups.getReference(id)!=nullptr) {
            id++;
        }
        else {
            idIsOk = true;
        }
    }
    groups.set(id, g);
    if (id != askedId) {
    } 
    g->id->setValue(id);
}

void Brain::unregisterGroup(Group* g) {
    if (groups.containsValue(g)) {
        groups.removeValue(g);
    }
}

void Brain::registerPreset(Preset* p, int id) {
    int askedId = id;
    if (presets.getReference(id) == p) { return; }
    if (presets.containsValue(p)) {
        presets.removeValue(p);
    }
    bool idIsOk = false;
    while (!idIsOk) {
        if (presets.contains(id) && presets.getReference(id) != nullptr) {
            id++;
        }
        else {
            idIsOk = true;
        }
    }
    presets.set(id, p);
    p->id->setValue(id);
    if (id != askedId) {
    }
}

void Brain::unregisterPreset(Preset* p) {
    if (presets.containsValue(p)) {
        presets.removeValue(p);
    }
}

void Brain::registerCuelist(Cuelist* c, int id) {
    int askedId = id;
    if (cuelists.getReference(id) == c) { return; }
    if (cuelists.containsValue(c)) {
        cuelists.removeValue(c);
    }
    bool idIsOk = false;
    while (!idIsOk) {
        if (cuelists.contains(id) && cuelists.getReference(id) != nullptr) {
            id++;
        }
        else {
            idIsOk = true;
        }
    }
    cuelists.set(id, c);
    c->id->setValue(id);
    if (id != askedId) {
    }
}

void Brain::unregisterCuelist(Cuelist* c) {
    if (cuelists.containsValue(c)) {
        cuelists.removeValue(c);
    }
}

void Brain::registerProgrammer(Programmer* c, int id) {
    int askedId = id;
    if (programmers.getReference(id) == c) { return; }
    if (programmers.containsValue(c)) {
        programmers.removeValue(c);
    }
    bool idIsOk = false;
    while (!idIsOk) {
        if (programmers.contains(id) && programmers.getReference(id) != nullptr) {
            id++;
        }
        else {
            idIsOk = true;
        }
    }
    programmers.set(id, c);
    c->id->setValue(id);
    if (id != askedId) {
    }
}

void Brain::unregisterProgrammer(Programmer* c) {
    if (programmers.containsValue(c)) {
        programmers.removeValue(c);
    }
}

void Brain::registerCurvePreset(CurvePreset* c, int id) {
    int askedId = id;
    if (curvePresets.getReference(id) == c) { return; }
    if (curvePresets.containsValue(c)) {
        curvePresets.removeValue(c);
    }
    bool idIsOk = false;
    while (!idIsOk) {
        if (curvePresets.contains(id) && curvePresets.getReference(id) != nullptr) {
            id++;
        }
        else {
            idIsOk = true;
        }
    }
    curvePresets.set(id, c);
    c->id->setValue(id);
    if (id != askedId) {
    }
}

void Brain::unregisterCurvePreset(CurvePreset* c) {
    if (curvePresets.containsValue(c)) {
        curvePresets.removeValue(c);
    }
}

void Brain::registerTimingPreset(TimingPreset* c, int id) {
    int askedId = id;
    if (timingPresets.getReference(id) == c) { return; }
    if (timingPresets.containsValue(c)) {
        timingPresets.removeValue(c);
    }
    bool idIsOk = false;
    while (!idIsOk) {
        if (timingPresets.contains(id) && timingPresets.getReference(id) != nullptr) {
            id++;
        }
        else {
            idIsOk = true;
        }
    }
    timingPresets.set(id, c);
    c->id->setValue(id);
    if (id != askedId) {
    }
}

void Brain::unregisterTimingPreset(TimingPreset* c) {
    if (timingPresets.containsValue(c)) {
        timingPresets.removeValue(c);
    }
}

void Brain::registerEffect(Effect* c, int id) {
    int askedId = id;
    if (effects.getReference(id) == c) { return; }
    if (effects.containsValue(c)) {
        effects.removeValue(c);
    }
    bool idIsOk = false;
    while (!idIsOk) {
        if (effects.contains(id) && effects.getReference(id) != nullptr) {
            id++;
        }
        else {
            idIsOk = true;
        }
    }
    effects.set(id, c);
    c->id->setValue(id);
    if (id != askedId) {
    }
}

void Brain::unregisterEffect(Effect* c) {
    if (effects.containsValue(c)) {
        effects.removeValue(c);
    }
}

void Brain::registerCarousel(Carousel* c, int id) {
    int askedId = id;
    if (carousels.getReference(id) == c) { return; }
    if (carousels.containsValue(c)) {
        carousels.removeValue(c);
    }
    bool idIsOk = false;
    while (!idIsOk) {
        if (carousels.contains(id) && carousels.getReference(id) != nullptr) {
            id++;
        }
        else {
            idIsOk = true;
        }
    }
    carousels.set(id, c);
    c->id->setValue(id);
    if (id != askedId) {
    }
}

void Brain::unregisterCarousel(Carousel* c) {
    if (carousels.containsValue(c)) {
        carousels.removeValue(c);
    }
}

void Brain::pleaseUpdate(Cuelist* c) {
    if (!cuelistPoolWaiting.contains(c)) {
        cuelistPoolWaiting.add(c);
    }
}

void Brain::pleaseUpdate(SubFixtureChannel* f) {
    if ( f == nullptr) {return; };
    if (!SubFixtureChannelPoolWaiting.contains(f)) {
        SubFixtureChannelPoolWaiting.add(f);
    }
}

void Brain::pleaseUpdate(Cue* c) {
    if (!cuePoolWaiting.contains(c)) {
        cuePoolWaiting.add(c);
    }
}

void Brain::pleaseUpdate(Programmer* c) {
    if (!programmerPoolWaiting.contains(c)) {
        programmerPoolWaiting.add(c);
    }
}

void Brain::pleaseUpdate(Effect* f) {
    if (!effectPoolWaiting.contains(f)) {
        effectPoolWaiting.add(f);
    }
}

void Brain::pleaseUpdate(Carousel* f) {
    if (!carouselPoolWaiting.contains(f)) {
        carouselPoolWaiting.add(f);
    }
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

SubFixture* Brain::getSubFixtureById(int id) {
    return SubFixtures.getReference(id);
}

Fixture* Brain::getFixtureById(int id) {
    return Fixtures.getReference(id);
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

Programmer* Brain::getProgrammerById(int id) {
    return programmers.getReference(id);
}

CurvePreset* Brain::getCurvePresetById(int id) {
    return curvePresets.getReference(id);
}

TimingPreset* Brain::getTimingPresetById(int id) {
    return timingPresets.getReference(id);
}

Effect* Brain::getEffectById(int id) {
    return effects.getReference(id);
}

Carousel* Brain::getCarouselById(int id) {
    return carousels.getReference(id);
}


void Brain::swoppedCuelist(Cuelist* c) {
    if (!swoppedCuelists.contains(c)) {
        swoppedCuelists.add(c);
    }
    for (int i = 0; i < swoppableChannels.size(); i++) {
        pleaseUpdate(swoppableChannels[i]);
    }
    isSwopping = true;
}

void Brain::unswoppedCuelist(Cuelist* c) {
    swoppedCuelists.removeAllInstancesOf(c);
    isSwopping = swoppedCuelists.size() > 0;
    for (int i = 0; i < swoppableChannels.size(); i++) {
        pleaseUpdate(swoppableChannels[i]);
    }
}