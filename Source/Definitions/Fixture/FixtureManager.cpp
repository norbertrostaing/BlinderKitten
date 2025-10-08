#include "Fixture.h"
#include "FixtureManager.h"
#include "Definitions/FixtureType/FixtureTypeChannel.h"
#include "Definitions/FixtureType/FixtureTypeVirtualChannel.h"
#include "UserInputManager.h"
#include "Brain.h"
#include "UI/PatchSheet/PatchSheet.h"

juce_ImplementSingleton(FixtureManager);

int compare(Fixture* A, Fixture* B) {
    return (int)A->id->getValue() - (int)B->id->getValue();
}

FixtureManager::FixtureManager() :
    BaseManager("Fixture")
    {
    itemDataType = "Fixture";
    selectItemWhenCreated = true;
    comparator.compareFunc = compare;
    autoReorderOnAdd = true;
}

FixtureManager::~FixtureManager()
{
}


void FixtureManager::addItemInternal(Fixture* o, var data)
{
}

void FixtureManager::removeItemInternal(Fixture* o)
{
    // o->removeObjectListener(this);
    if (!Brain::getInstance()->isClearing && !Brain::getInstance()->loadingIsRunning) {
        UserInputManager::getInstance()->processInput("ClearAll");
    }

}

void FixtureManager::askForMoveBefore(BaseItem*)
{
}

void FixtureManager::askForMoveAfter(BaseItem*)
{
}

void FixtureManager::askForDuplicateItem(BaseItem* item)
{
}

void FixtureManager::setItemIndex(Fixture* item, int newIndex, bool addToUndo)
{
    BaseManager::setItemIndex(item, newIndex, addToUndo);
}

void FixtureManager::invertValueChanged(FixtureTypeChannel* ftc)
{
    for (int i = 0; i < items.size(); i++) {
        Fixture* f = items[i];
        for (auto itSf = f->subFixtures.begin(); itSf != f->subFixtures.end(); itSf.next()) {
            SubFixture* sf = itSf.getValue();
            if (sf == nullptr) continue;
            for (auto itSfc = sf->channelsMap.begin(); itSfc != sf->channelsMap.end(); itSfc.next()) {
                SubFixtureChannel *sfc = itSfc.getValue();
                if (sfc->parentFixtureTypeChannel == ftc) {
                    sfc->invertOutput = ftc->invertOutput->boolValue();
                    Brain::getInstance()->pleaseUpdate(sfc);
                }
            }
        }
    }
}

void FixtureManager::defaultValueChanged(FixtureTypeDMXChannel* ftc)
{
    for (int i = 0; i < items.size(); i++) {
        Fixture* f = items[i];
        for (int x = 0; x < f->fixtureDMXChannels.size(); x++){
            FixtureDMXChannel* d = f->fixtureDMXChannels[x];
            if (d->fixtureTypeDMXChannel == ftc) {
                for (int s = 0; s < d->logicalChannels.size(); s++){
                    SubFixtureChannel* sfc = d->logicalChannels[s];
                    Brain::getInstance()->pleaseUpdate(sfc);
                }
            }
        }
    }
}

void FixtureManager::defaultValueChanged(FixtureTypeVirtualChannel* ftc)
{
    for (int i = 0; i < items.size(); i++) {
        Fixture* f = items[i];
        for (auto itSf = f->subFixtures.begin(); itSf != f->subFixtures.end(); itSf.next()) {
            SubFixture* sf = itSf.getValue();
            for (auto itSfc = sf->channelsMap.begin(); itSfc != sf->channelsMap.end(); itSfc.next()) {
                SubFixtureChannel* sfc = itSfc.getValue();
                if (sfc->parentFixtureTypeVirtualChannel == ftc) {
                    Brain::getInstance()->pleaseUpdate(sfc);
                }
            }
        }
    }
}

void FixtureManager::reorderItems()
{
    BaseManager::reorderItems();
}



