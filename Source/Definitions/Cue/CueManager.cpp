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
};

