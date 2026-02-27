#include "CueManager.h"
#include "../../Brain.h"
#include "Definitions/Cuelist/Cuelist.h"

CueManager::CueManager() :
    BaseManager("Cues")
{
    itemDataType = "Cue";
    selectItemWhenCreated = false;       
}

CueManager::~CueManager()
{
}

void CueManager::askForMoveBefore(BaseItem* c) {
    BaseManager::askForMoveBefore(c);
    Cue* c1 = (Cue*)c;
    Cue* c2 = items[items.indexOf(c1) + 1];
    float c1Id = c1->id->getValue();
    float c2Id = c2->id->getValue();
    c1->id->setValue(c2Id);
    c2->id->setValue(c1Id);
    parentCuelist->sendChangeMessage();
};

void CueManager::askForMoveAfter(BaseItem* c) {
    BaseManager::askForMoveAfter(c);
    Cue* c1 = (Cue*)c;
    Cue* c2 = items[items.indexOf(c1) - 1];
    float c1Id = c1->id->getValue();
    float c2Id = c2->id->getValue();
    c1->id->setValue(c2Id);
    c2->id->setValue(c1Id);
    parentCuelist->sendChangeMessage();
};

void CueManager::addItemInternal(Cue* c, var data) {
    float maxId = 0;
    for (Cue* cue : items) {
        if (c != cue) maxId = jmax(maxId, cue->id->floatValue());
    }
    if (maxId != 0 && c->id->floatValue() == 1) {
        c->id->setValue(floor(maxId+1));
    }
    //reorderItems();
    //correctCueIds();
    parentCuelist->sendChangeMessage();
}

void CueManager::askForDuplicateItem(BaseItem* item)
{
    BaseManager::askForDuplicateItem(item);
    correctCueIds();
    parentCuelist->sendChangeMessage();
}

void CueManager::correctCueIds()
{
    if (items.size() == 0) return;
    float lastValidId = items[0]->id->floatValue();
    Array<Cue*> invalidCueIds;

    for (int i = 1; i < items.size(); i++) {
        float id = items[i]->id->getValue();
        float prevId = items[i - 1]->id->getValue();

        if (id > prevId) { // tout va bien
            for (int j = 0; j < invalidCueIds.size(); j++) {
                invalidCueIds[j]->id->setValue(jmap(float(j+1),0.0f, float(invalidCueIds.size()+1), lastValidId, id));
            }
            invalidCueIds.clear();
            lastValidId = id;
        }
        else { // cheh
            invalidCueIds.add(items[i]);
        }
    }
    for (int j = 0; j < invalidCueIds.size(); j++) {
        invalidCueIds[j]->id->setValue(lastValidId+1+j);
    }
 
 }

void CueManager::setItemIndex(Cue* item, int newIndex, bool addToUndo)
{
    BaseManager::setItemIndex(item, newIndex,  addToUndo);
    if (items.size() == 1) {return;}
    if (newIndex == 0 && items.size() > 1) {
        double next = items[1]->id->getValue();
        item->id->setValue(calcId(0,next));
    }
    else if (items.size() == newIndex + 1) {
        double prev = items[newIndex-1]->id->getValue();
        if (abs(prev) != prev) { prev = floor(prev); }
        item->id->setValue(prev+1);
    }
    else {
        double next = items[newIndex + 1]->id->getValue();
        double prev = items[newIndex - 1]->id->getValue();
        item->id -> setValue(calcId(prev, next));
    }
    parentCuelist->sendChangeMessage();
}

double CueManager::calcId(double prev, double next)
{
    double delta = (next - prev) / 2;
    if (delta <= 0) return prev + 1;
    float ret = prev + (delta/2);
    if (delta >= 1) { ret = floor( prev + 1); }
    else if (delta >= 0.1) { ret = prev + 0.1; }
    else if (delta >= 0.01) { ret = prev + 0.01; }
    else if (delta >= 0.001) { ret = prev + 0.001; }
    return ret;
}

void CueManager::removeItemInternal(Cue* c)
{
    if (this->parentContainer != nullptr) {
        if (parentCuelist->cueA == c) {
            parentCuelist->cueA = nullptr;
        }
    }
    parentCuelist->sendChangeMessage();
}



