/*
  ==============================================================================

    Brain.cpp
    Created: 14 Nov 2021 10:42:56pm
    Author:  No

  ==============================================================================
*/


#include "Brain.h"
#include "Definitions/ChannelValue.h"
#include "UI/VirtualFaders/VirtualFaderColGrid.h"

juce_ImplementSingleton(Brain);

Brain::Brain() :
    Thread("BKBrain") 
{
    startThread();
}

Brain :: ~Brain() {
    clear();
    signalThreadShouldExit();
    waitForThreadToExit(100);
    clearSingletonInstance();
}

void Brain::clear()
{
    ScopedLock lock(usingCollections);
    subFixtures.clear();
    groups.clear();
    fixtures.clear();
    presets.clear();
    cuelists.clear();
    programmers.clear();
    curvePresets.clear();
    timingPresets.clear();
    effects.clear();
    carousels.clear();
    mappers.clear();
    cuelistPoolUpdating.clear();
    cuelistPoolWaiting.clear();
    subFixtureChannelPoolUpdating.clear();
    subFixtureChannelPoolWaiting.clear();
    cuePoolUpdating.clear();
    cuePoolWaiting.clear();
    programmerPoolUpdating.clear();
    programmerPoolWaiting.clear();
    effectPoolUpdating.clear();
    effectPoolWaiting.clear();
    carouselPoolUpdating.clear();
    carouselPoolWaiting.clear();
    mapperPoolUpdating.clear();
    mapperPoolWaiting.clear();
}

void Brain::clearUpdates()
{
    ScopedLock lock(usingCollections);
    cuelistPoolUpdating.clear();
    cuelistPoolWaiting.clear();
    subFixtureChannelPoolUpdating.clear();
    subFixtureChannelPoolWaiting.clear();
    cuePoolUpdating.clear();
    cuePoolWaiting.clear();
    programmerPoolUpdating.clear();
    programmerPoolWaiting.clear();
    effectPoolUpdating.clear();
    effectPoolWaiting.clear();
    carouselPoolUpdating.clear();
    carouselPoolWaiting.clear();
    mapperPoolUpdating.clear();
    mapperPoolWaiting.clear();

}

void Brain::run() {
    while(!threadShouldExit()) {
        brainLoop();
        wait(10);
    }
}

void Brain::brainLoop() {


    if (pleaseClearProgrammer) {
        pleaseClearProgrammer = false;
        if (Programmer* p = getProgrammerById(1)) {
            p->clearCurrent();
        }
    }


    now = Time::getMillisecondCounterHiRes();
    if (cuePoolWaiting.size() > 0) {
        ScopedLock lock(usingCollections);
        for (int i = 0; i < cuePoolWaiting.size(); i++) {
            cuePoolUpdating.push_back(cuePoolWaiting.at(i));
        }
        cuePoolWaiting.clear();
    }
    for (int i = 0; i < cuePoolUpdating.size(); i++) {
        cuePoolUpdating.at(i)->update(now);
    }
    cuePoolUpdating.clear();

    if (cuelistPoolWaiting.size() > 0) {
        ScopedLock lock(usingCollections);
        for (int i = 0; i < cuelistPoolWaiting.size(); i++) {
            cuelistPoolUpdating.push_back(cuelistPoolWaiting.at(i));
        }
        cuelistPoolWaiting.clear();
    }
    for (int i = 0; i < cuelistPoolUpdating.size(); i++) {
        cuelistPoolUpdating.at(i)->update();
    }
    cuelistPoolUpdating.clear();

    if (effectPoolWaiting.size() > 0) {
        ScopedLock lock(usingCollections);
        for (int i = 0; i < effectPoolWaiting.size(); i++) {
            effectPoolUpdating.push_back(effectPoolWaiting.at(i));
        }
        effectPoolWaiting.clear();
    }
    for (int i = 0; i < effectPoolUpdating.size(); i++) {
        effectPoolUpdating.at(i)->update(now);
    }
    effectPoolUpdating.clear();

    if (carouselPoolWaiting.size() > 0) {
        ScopedLock lock(usingCollections);
        for (int i = 0; i < carouselPoolWaiting.size(); i++) {
            carouselPoolUpdating.push_back(carouselPoolWaiting.at(i));
        }
        carouselPoolWaiting.clear();
    }
    for (int i = 0; i < carouselPoolUpdating.size(); i++) {
        carouselPoolUpdating.at(i)->update(now);
    }
    carouselPoolUpdating.clear();

    if (mapperPoolWaiting.size() > 0) {
        ScopedLock lock(usingCollections);
        for (int i = 0; i < mapperPoolWaiting.size(); i++) {
            mapperPoolUpdating.push_back(mapperPoolWaiting.at(i));
        }
        mapperPoolWaiting.clear();
    }
    for (int i = 0; i < mapperPoolUpdating.size(); i++) {
        mapperPoolUpdating.at(i)->update(now);
    }
    mapperPoolUpdating.clear();

    if (programmerPoolWaiting.size() > 0) {
        ScopedLock lock(usingCollections);
        for (int i = 0; i < programmerPoolWaiting.size(); i++) {
            programmerPoolUpdating.push_back(programmerPoolWaiting.at(i));
        }
        programmerPoolWaiting.clear();
    }
    for (int i = 0; i < programmerPoolUpdating.size(); i++) {
        programmerPoolUpdating.at(i)->update(now);
    }
    programmerPoolUpdating.clear();

    {
        ScopedLock lock(usingCollections);
        for (int i = 0; i < subFixtureChannelPoolWaiting.size(); i++) {
            subFixtureChannelPoolUpdating.push_back(subFixtureChannelPoolWaiting.at(i));
        }
    }
    subFixtureChannelPoolWaiting.clear();
    for (int i = 0; i < subFixtureChannelPoolUpdating.size(); i++) {
        // bug ici
        if (subFixtureChannelPoolUpdating.at(i) != nullptr && !subFixtureChannelPoolUpdating.at(i)->isDeleted) {
            subFixtureChannelPoolUpdating.at(i)->updateVal(now);
        }
    }
    subFixtureChannelPoolUpdating.clear();

    {
        for (int i = 0; i < runningTasks.size(); i++) {
            runningTasks[i]->update(now);
        }
        ScopedLock lock(usingCollections);
        for (int i = runningTasks.size() - 1; i >= 0; i--) {
            if (runningTasks[i]->isEnded) {
                runningTasks.remove(i);
            }
        }
    }

    if (virtualFadersNeedUpdate) {
        virtualFadersNeedUpdate = false;
        VirtualFaderColGrid::getInstance()->updateSlidersValues();

    }

    //double delta = Time::getMillisecondCounterHiRes() - now;
    //LOG(delta);

}

void Brain::registerSubFixture(SubFixture* f, int id) {
    int askedId = id;
    if (subFixtures.getReference(id) == f) { return; }
    if (subFixtures.containsValue(f)) {
        subFixtures.removeValue(f);
    }
    bool idIsOk = false;
    while (!idIsOk) {
        if (subFixtures.contains(id) && subFixtures.getReference(id) != nullptr) {
            id++;
        }
        else {
            idIsOk = true;
        }
    }
    subFixtures.set(id, f);
    f->subId =id;
    if (id != askedId) {
    }
}

void Brain::unregisterSubFixture(SubFixture* f) {
    if (subFixtures.containsValue(f)) {
        subFixtures.removeValue(f);
    }
}

void Brain::registerFixture(Fixture* p, int id, bool swap) {
    int askedId = id;
    if (fixtures.getReference(id) == p) { return; }
    if (fixtures.containsValue(p)) {
        fixtures.removeValue(p);
    }
    bool idIsOk = false;
    if (swap && p->registeredId != 0) {
        if (fixtures.contains(id) && fixtures.getReference(id) != nullptr) {
            Fixture* presentItem = fixtures.getReference(id);
            unregisterFixture(p);
            registerFixture(presentItem, p->registeredId, false);
        }
    }
    while (!idIsOk) {
        if (fixtures.contains(id) && fixtures.getReference(id) != nullptr) {
            id++;
        }
        else {
            idIsOk = true;
        }
    }
    fixtures.set(id, p);
    p->id->setValue(id);
    p->registeredId = id;
    if (id != askedId) {
    }
}

void Brain::unregisterFixture(Fixture* d) {
    if (fixtures.containsValue(d)) {
        fixtures.removeValue(d);
    }
}

void Brain::registerGroup(Group* p, int id, bool swap) {
    int askedId = id;
    if (groups.getReference(id) == p) { return; }
    if (groups.containsValue(p)) {
        groups.removeValue(p);
    }
    bool idIsOk = false;
    if (swap && p->registeredId != 0) {
        if (groups.contains(id) && groups.getReference(id) != nullptr) {
            Group* presentItem = groups.getReference(id);
            unregisterGroup(p);
            registerGroup(presentItem, p->registeredId, false);
        }
    }
    while (!idIsOk) {
        if (groups.contains(id) && groups.getReference(id) != nullptr) {
            id++;
        }
        else {
            idIsOk = true;
        }
    }
    groups.set(id, p);
    p->id->setValue(id);
    p->registeredId = id;
    if (id != askedId) {
    }
}

void Brain::unregisterGroup(Group* g) {
    if (groups.containsValue(g)) {
        groups.removeValue(g);
    }
}

void Brain::registerPreset(Preset* p, int id, bool swap) {
    int askedId = id;
    if (presets.getReference(id) == p) { return; }
    if (presets.containsValue(p)) {
        presets.removeValue(p);
    }
    bool idIsOk = false;
    if (swap && p->registeredId != 0) {
        if (presets.contains(id) && presets.getReference(id) != nullptr ){
            Preset* presentItem = presets.getReference(id);
            unregisterPreset(p);
            registerPreset(presentItem, p->registeredId, false);
        }
    }
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
    p->registeredId = id;
    if (id != askedId) {
    }
}

void Brain::unregisterPreset(Preset* p) {
    if (presets.containsValue(p)) {
        presets.removeValue(p);
    }
}

void Brain::registerCuelist(Cuelist* p, int id, bool swap) {
    int askedId = id;
    //LOG("register Cuelist");
    if (cuelists.getReference(id) == p) { return; }
    if (cuelists.containsValue(p)) {
        cuelists.removeValue(p);
    }
    bool idIsOk = false;
    if (swap && p->registeredId != 0) {
        if (cuelists.contains(id) && cuelists.getReference(id) != nullptr) {
            Cuelist* presentItem = cuelists.getReference(id);
            unregisterCuelist(p);
            registerCuelist(presentItem, p->registeredId, false);
        }
    }
    while (!idIsOk) {
        if (cuelists.contains(id) && cuelists.getReference(id) != nullptr) {
            id++;
        }
        else {
            idIsOk = true;
        }
    }
    cuelists.set(id, p);
    p->id->setValue(id);
    p->registeredId = id;
    if (id != askedId) {
    }
}

void Brain::unregisterCuelist(Cuelist* c) {
    if (cuelists.containsValue(c)) {
        cuelists.removeValue(c);
    }
}

void Brain::registerProgrammer(Programmer* p, int id, bool swap) {
    int askedId = id;
    if (programmers.getReference(id) == p) { return; }
    if (programmers.containsValue(p)) {
        programmers.removeValue(p);
    }
    bool idIsOk = false;
    if (swap && p->registeredId != 0) {
        if (programmers.contains(id) && programmers.getReference(id) != nullptr) {
            Programmer* presentItem = programmers.getReference(id);
            unregisterProgrammer(p);
            registerProgrammer(presentItem, p->registeredId, false);
        }
    }
    while (!idIsOk) {
        if (programmers.contains(id) && programmers.getReference(id) != nullptr) {
            id++;
        }
        else {
            idIsOk = true;
        }
    }
    programmers.set(id, p);
    p->id->setValue(id);
    p->registeredId = id;
    if (id != askedId) {
    }
}


void Brain::unregisterProgrammer(Programmer* c) {
    if (programmers.containsValue(c)) {
        programmers.removeValue(c);
    }
}

void Brain::registerCurvePreset(CurvePreset* p, int id, bool swap) {
    int askedId = id;
    if (curvePresets.getReference(id) == p) { return; }
    if (curvePresets.containsValue(p)) {
        curvePresets.removeValue(p);
    }
    bool idIsOk = false;
    if (swap && p->registeredId != 0) {
        if (curvePresets.contains(id) && curvePresets.getReference(id) != nullptr) {
            CurvePreset* presentItem = curvePresets.getReference(id);
            unregisterCurvePreset(p);
            registerCurvePreset(presentItem, p->registeredId, false);
        }
    }
    while (!idIsOk) {
        if (curvePresets.contains(id) && curvePresets.getReference(id) != nullptr) {
            id++;
        }
        else {
            idIsOk = true;
        }
    }
    curvePresets.set(id, p);
    p->id->setValue(id);
    p->registeredId = id;
    if (id != askedId) {
    }
}


void Brain::unregisterCurvePreset(CurvePreset* c) {
    if (curvePresets.containsValue(c)) {
        curvePresets.removeValue(c);
    }
}

void Brain::registerTimingPreset(TimingPreset* p, int id, bool swap) {
    int askedId = id;
    if (timingPresets.getReference(id) == p) { return; }
    if (timingPresets.containsValue(p)) {
        timingPresets.removeValue(p);
    }
    bool idIsOk = false;
    if (swap && p->registeredId != 0) {
        if (timingPresets.contains(id) && timingPresets.getReference(id) != nullptr) {
            TimingPreset* presentItem = timingPresets.getReference(id);
            unregisterTimingPreset(p);
            registerTimingPreset(presentItem, p->registeredId, false);
        }
    }
    while (!idIsOk) {
        if (timingPresets.contains(id) && timingPresets.getReference(id) != nullptr) {
            id++;
        }
        else {
            idIsOk = true;
        }
    }
    timingPresets.set(id, p);
    p->id->setValue(id);
    p->registeredId = id;
    if (id != askedId) {
    }
}


void Brain::unregisterTimingPreset(TimingPreset* c) {
    if (timingPresets.containsValue(c)) {
        timingPresets.removeValue(c);
    }
}

void Brain::registerEffect(Effect* p, int id, bool swap) {
    int askedId = id;
    if (effects.getReference(id) == p) { return; }
    if (effects.containsValue(p)) {
        effects.removeValue(p);
    }
    bool idIsOk = false;
    if (swap && p->registeredId != 0) {
        if (effects.contains(id) && effects.getReference(id) != nullptr) {
            Effect* presentItem = effects.getReference(id);
            unregisterEffect(p);
            registerEffect(presentItem, p->registeredId, false);
        }
    }
    while (!idIsOk) {
        if (effects.contains(id) && effects.getReference(id) != nullptr) {
            id++;
        }
        else {
            idIsOk = true;
        }
    }
    effects.set(id, p);
    p->id->setValue(id);
    p->registeredId = id;
    if (id != askedId) {
    }
}


void Brain::unregisterEffect(Effect* c) {
    if (effects.containsValue(c)) {
        effects.removeValue(c);
    }
}

void Brain::registerCarousel(Carousel* p, int id, bool swap) {
    int askedId = id;
    if (carousels.getReference(id) == p) { return; }
    if (carousels.containsValue(p)) {
        carousels.removeValue(p);
    }
    bool idIsOk = false;
    if (swap && p->registeredId != 0) {
        if (carousels.contains(id) && carousels.getReference(id) != nullptr) {
            Carousel* presentItem = carousels.getReference(id);
            unregisterCarousel(p);
            registerCarousel(presentItem, p->registeredId, false);
        }
    }
    while (!idIsOk) {
        if (carousels.contains(id) && carousels.getReference(id) != nullptr) {
            id++;
        }
        else {
            idIsOk = true;
        }
    }
    carousels.set(id, p);
    p->id->setValue(id);
    p->registeredId = id;
    if (id != askedId) {
    }
}


void Brain::unregisterCarousel(Carousel* c) {
    if (carousels.containsValue(c)) {
        carousels.removeValue(c);
    }
}

void Brain::registerMapper(Mapper* p, int id, bool swap) {
    int askedId = id;
    if (mappers.getReference(id) == p) { return; }
    if (mappers.containsValue(p)) {
        mappers.removeValue(p);
    }
    bool idIsOk = false;
    if (swap && p->registeredId != 0) {
        if (mappers.contains(id) && mappers.getReference(id) != nullptr) {
            Mapper* presentItem = mappers.getReference(id);
            unregisterMapper(p);
            registerMapper(presentItem, p->registeredId, false);
        }
    }
    while (!idIsOk) {
        if (mappers.contains(id) && mappers.getReference(id) != nullptr) {
            id++;
        }
        else {
            idIsOk = true;
        }
    }
    mappers.set(id, p);
    p->id->setValue(id);
    p->registeredId = id;
    if (id != askedId) {
    }
}


void Brain::unregisterMapper(Mapper* c) {
    if (mappers.containsValue(c)) {
        mappers.removeValue(c);
    }
}

void Brain::pleaseUpdate(Cuelist* c) {
    if (c == nullptr) {return;}
    ScopedLock lock(usingCollections);
    if (std::find(cuelistPoolWaiting.begin(), cuelistPoolWaiting.end(), c) != cuelistPoolWaiting.end()) {
    }
    else {
        cuelistPoolWaiting.push_back(c);
    }
}

void Brain::pleaseUpdate(SubFixtureChannel* f) {
    if (f == nullptr || f ->isDeleted || f->objectType != "SubFixtureChannel") { return; }
    ScopedLock lock(usingCollections);
    if (f == nullptr) { return; };
    if (std::find(subFixtureChannelPoolWaiting.begin(), subFixtureChannelPoolWaiting.end(), f) != subFixtureChannelPoolWaiting.end()) {}
    else {
        subFixtureChannelPoolWaiting.push_back(f);
    }
}

void Brain::pleaseUpdate(Cue* c) {
    if (c == nullptr || c->objectType != "Cue") { return; }
    ScopedLock lock(usingCollections);
    if (std::find(cuePoolWaiting.begin(), cuePoolWaiting.end(), c) != cuePoolWaiting.end()) {}
    else {
        cuePoolWaiting.push_back(c);
    }
}

void Brain::pleaseUpdate(Programmer* c) {
    if (c == nullptr || c->objectType != "Programmer") { return; }
    ScopedLock lock(usingCollections);
    if (std::find(programmerPoolWaiting.begin(), programmerPoolWaiting.end(), c) != programmerPoolWaiting.end()) {}
    else {
        programmerPoolWaiting.push_back(c);
    }
}

void Brain::pleaseUpdate(Effect* f) {
    if (f == nullptr || f->objectType != "Effect") { return; }
    ScopedLock lock(usingCollections);
    if (std::find(effectPoolWaiting.begin(), effectPoolWaiting.end(), f) != effectPoolWaiting.end()) {}
    else {
        effectPoolWaiting.push_back(f);
    }
}

void Brain::pleaseUpdate(Carousel* f) {
    if (f == nullptr || f->objectType != "Carousel") { return; }
    ScopedLock lock(usingCollections);
    if (std::find(carouselPoolWaiting.begin(), carouselPoolWaiting.end(), f) != carouselPoolWaiting.end()) {}
    else {
        carouselPoolWaiting.push_back(f);
    }
}

void Brain::pleaseUpdate(Mapper* f) {
    if (f == nullptr || f->objectType != "Mapper") { return; }
    ScopedLock lock(usingCollections);
    if (std::find(mapperPoolWaiting.begin(), mapperPoolWaiting.end(), f) != mapperPoolWaiting.end()) {}
    else {
        mapperPoolWaiting.push_back(f);
    }
}

void Brain::grandMasterChanged()
{
    for (int i = 0; i < grandMasterChannels.size(); i++) {
        pleaseUpdate(grandMasterChannels[i]);
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

void Brain::startTask(Task* t, double startTime)
{
    ScopedLock lock(usingCollections);

    String actionType = "";
    String targetType = t->targetType->getValue();
    Array<int> targetIds;

    if (t->targetThru->getValue()) {
        int tId = t -> targetId -> getValue();
        int tIdTo = t -> targetIdTo -> getValue();
        int from = jmin(tId, tIdTo);
        int to = jmax(tId, tIdTo);
        for (int i = from; i <= to; i++) {
            targetIds.add(i);
        }
    }
    else {
        targetIds.add(t->targetId->getValue());
    }
    for (int i = 0; i < targetIds.size(); i++) {
        bool valid = false;
        double startValue = 0;
        double endValue = 1;
        int targetId = targetIds[i];
        if (targetType == "cuelist") {
            actionType = t->cuelistAction->getValue();
            Cuelist* target = getCuelistById(targetId);
            if (target != nullptr) {
                valid = true;
                if (actionType == "htplevel") {
                    startValue = target->HTPLevel->getValue();
                    endValue = t->targetValue->getValue();
                } else if (actionType == "ltplevel") {
                    startValue = target->LTPLevel->getValue();
                    endValue = t->targetValue->getValue();
                } else if (actionType == "flashLevel") {
                    startValue = target->FlashLevel->getValue();
                    endValue = t->targetValue->getValue();
                }
            }
        }
        else if (targetType == "effect") {
            actionType = t->effectAction->getValue();
            Effect* target = getEffectById(targetId);
            if (target != nullptr) {
                valid = true;
                if (actionType == "size") {
                    startValue = target->sizeValue->getValue();
                    endValue = t->targetValue->getValue();
                }
                else if (actionType == "speed") {
                    startValue = target->speed->getValue();
                    endValue = t->targetValue->getValue();
                }
            }
        }
        else if (targetType == "carousel") {
            actionType = t->carouselAction->getValue();
            Carousel* target = getCarouselById(targetId);
            if (target != nullptr) {
                valid = true;
                if (actionType == "size") {
                    startValue = target->sizeValue->getValue();
                    endValue = t->targetValue->getValue();
                }
                else if (actionType == "speed") {
                    startValue = target->speed->getValue();
                    endValue = t->targetValue->getValue();
                }
            }
        }
        else if (targetType == "mapper") {
            actionType = t->mapperAction->getValue();
            Mapper* target = getMapperById(targetId);
            if (target != nullptr) {
                valid = true;
                if (actionType == "size") {
                    startValue = target->sizeValue->getValue();
                    endValue = t->targetValue->getValue();
                }
            }
        }

        if (valid) {
            RunningTask* rt = runningTasks.add(new RunningTask());
            rt->actionType = actionType;
            rt->targetType = targetType;
            rt->targetId = targetId;

            rt->delay = (double)t->delay->getValue() * 1000;
            rt->fade = (double)t->fade->getValue() * 1000;

            rt->TSInit = startTime;
            rt->TSStart = startTime + rt->delay;
            rt->TSEnd = startTime + rt->delay + rt->fade;

            rt->startValue = startValue;
            rt->endValue = endValue;
            rt->isEnded = false;
            //LOG(rt->endValue);
        }
    }

}

SubFixture* Brain::getSubFixtureById(int id) {
    if (subFixtures.contains(id)) {
        return subFixtures.getReference(id);
    }
    else {
        return nullptr;
    }
}

Fixture* Brain::getFixtureById(int id) {
    if (fixtures.contains(id)) {
        return fixtures.getReference(id);
    }
    else {
        return nullptr;
    }
}

Group* Brain::getGroupById(int id) {
    if (groups.contains(id)) {
        return groups.getReference(id);
    }
    else {
        return nullptr;
    }
}

Preset* Brain::getPresetById(int id) {
    if (presets.contains(id)) {
        return presets.getReference(id);
    }
    else {
        return nullptr;
    }
}

Cuelist* Brain::getCuelistById(int id) {
    if (cuelists.contains(id)) {
        return cuelists.getReference(id);
    }
    else {
        return nullptr;
    }
}

Programmer* Brain::getProgrammerById(int id) {
    if (programmers.contains(id)) {
        return programmers.getReference(id);
    }
    else {
        return nullptr;
    }
}

CurvePreset* Brain::getCurvePresetById(int id) {
    if (curvePresets.contains(id)) {
        return curvePresets.getReference(id);
    }
    else {
        return nullptr;
    }
}

TimingPreset* Brain::getTimingPresetById(int id) {
    if (timingPresets.contains(id)) {
        return timingPresets.getReference(id);
    }
    else {
        return nullptr;
    }
}

Effect* Brain::getEffectById(int id) {
    if (effects.contains(id)) {
        return effects.getReference(id);
    }
    else {
        return nullptr;
    }
}

Carousel* Brain::getCarouselById(int id) {
    if (carousels.contains(id)) {
        return carousels.getReference(id);
    }
    else {
        return nullptr;
    }
}

Mapper* Brain::getMapperById(int id) {
    if (mappers.contains(id)) {
        return mappers.getReference(id);
    }
    else {
        return nullptr;
    }
}


void Brain::swoppedCuelist(Cuelist* c) {
    if (!swoppedCuelists.contains(c)) {
        swoppedCuelists.add(c);
    }
    for (int i = 0; i < swoppableChannels.size(); i++) {
        pleaseUpdate(swoppableChannels.getReference(i));
    }
    isSwopping = true;
}

void Brain::unswoppedCuelist(Cuelist* c) {
    {
        ScopedLock lock(Brain::getInstance()->usingCollections);
        while (swoppedCuelists.indexOf(c) >= 0) {
            swoppedCuelists.removeAllInstancesOf(c);
        }
    }
    isSwopping = swoppedCuelists.size() > 0;
    for (int i = 0; i < swoppableChannels.size(); i++) {
        pleaseUpdate(swoppableChannels.getReference(i));
    }
}


void Brain::updateAllChannels() {
    for (auto it = fixtures.begin(); it != fixtures.end(); it.next()) {
        Fixture* fixt = it.getValue();
        for (auto it2 = fixt->subFixtures.begin(); it2 != fixt->subFixtures.end(); it2.next()) {
            SubFixture* subFixt = it2.getValue();
            for (auto it3 = subFixt->channelsMap.begin(); it3 != subFixt->channelsMap.end(); it3.next()) {
                pleaseUpdate(it3.getValue());
            }
        }
    }
}

void Brain::goAllLoadedCuelists() {
    for (auto it = cuelists.begin(); it != cuelists.end(); it.next()) {
        Cuelist* c = it.getValue();
        Cue* cueB = dynamic_cast<Cue*>(c->nextCue->targetContainer.get());
        if (cueB != nullptr) {
            c->go();
        }
        else if ((int)c->nextCueId->getValue() > 0) {
            c->go();
        }
    }
}

void Brain::killAllCuelists()
{
    for (auto it = cuelists.begin(); it != cuelists.end(); it.next()) {
        Cuelist* c = it.getValue();
        c->kill();
    }
}

void Brain::offAllCuelists()
{
    for (auto it = cuelists.begin(); it != cuelists.end(); it.next()) {
        Cuelist* c = it.getValue();
        c->off();
    }
}

void Brain::stopAllEffects()
{
    for (auto it = effects.begin(); it != effects.end(); it.next()) {
        Effect* f = it.getValue();
        f->stop();
    }
}

void Brain::stopAllCarousels()
{
    for (auto it = carousels.begin(); it != carousels.end(); it.next()) {
        Carousel* f = it.getValue();
        f->stop();
    }
}

void Brain::resetRandomSeed(int seed)
{
    srand(seed);
}
