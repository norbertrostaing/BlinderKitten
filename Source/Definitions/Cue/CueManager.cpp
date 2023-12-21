#include "CueManager.h"
#include "../../Brain.h"

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
};

void CueManager::askForMoveAfter(BaseItem* c) {
    BaseManager::askForMoveAfter(c);
    Cue* c1 = (Cue*)c;
    Cue* c2 = items[items.indexOf(c1) - 1];
    float c1Id = c1->id->getValue();
    float c2Id = c2->id->getValue();
    c1->id->setValue(c2Id);
    c2->id->setValue(c1Id);
};

void CueManager::addItemInternal(Cue* c, var data) {
    if ((float)c->id->getValue() != 0) {return;}
    float newId = 1;
    if (items.size() > 0) {
        Cue* lastCue = items[items.size() - 1];
        newId = (float)lastCue->id->getValue() + 1;
    } 
    BaseManager::addItemInternal(c, data);
    c->id->setValue(newId);
}

void CueManager::askForDuplicateItem(BaseItem* item)
{
    BaseManager::askForDuplicateItem(item);
    correctCueIds();
}

void CueManager::correctCueIds()
{
    for (int i = 1; i < items.size(); i++) {
        float id = items[i]->id->getValue();
        float prevId = items[i-1]->id->getValue();
        if (id == prevId) {
            if (i == items.size() - 1) { // last element
                items[i]->id->setValue(id+1);
            }
            else {
                float nextId = items[i+1]->id->getValue();
                if (nextId == prevId) {
                    LOGERROR("Your cue IDs are the same, please consider renumber your cues.");
                    return;
                }
                else {
                    if (nextId < prevId) {
                        LOGERROR("Your cue IDs are not ordered, please consider renumber your cues.");
                        return;
                    }
                    items[i]->id->setValue(calcId(prevId, nextId));
                }
            }
        }
    }
}

void CueManager::setItemIndex(Cue* item, int newIndex, bool addToUndo)
{
    BaseManager::setItemIndex(item, newIndex,  addToUndo);
    double id = item->id->getValue();
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
}

double CueManager::calcId(double prev, double next)
{
    double delta = (next - prev) / 2;
    if (delta >= 1) { delta = 1; }
    else if (delta >= 0.1) { delta = 0.1; }
    else if (delta >= 0.01) { delta = 0.01; }
    else if (delta >= 0.001) { delta = 0.001; }
    return prev + delta;
}

void CueManager::removeItemInternal(Cue* c)
{
    if (this->parentContainer != nullptr) {
        Cuelist* parentCuelist = dynamic_cast<Cuelist*>(this->parentContainer.get());
        if (parentCuelist->cueA == c) {
            parentCuelist->cueA = nullptr;
        }
    }

}



