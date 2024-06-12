#include "Fixture.h"
#include "FixtureManager.h"
#include "UI/GridView/FixtureGridView.h"
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
}

FixtureManager::~FixtureManager()
{
}


void FixtureManager::addItemInternal(Fixture* o, var data)
{
    reorderItems();
    FixtureGridView::getInstance()->updateCells();
    PatchSheet::getInstance()->rebuildLines();
}

void FixtureManager::removeItemInternal(Fixture* o)
{
    // o->removeObjectListener(this);
    if (!Brain::getInstance()->isClearing && !Brain::getInstance()->loadingIsRunning) {
        UserInputManager::getInstance()->processInput("ClearAll");
    }
    FixtureGridView::getInstance()->updateCells();
    PatchSheet::getInstance()->rebuildLines();

}

void FixtureManager::askForMoveBefore(BaseItem*)
{
    PatchSheet::getInstance()->rebuildLines();
}

void FixtureManager::askForMoveAfter(BaseItem*)
{
    PatchSheet::getInstance()->rebuildLines();
}

void FixtureManager::askForDuplicateItem(BaseItem* item)
{
    PatchSheet::getInstance()->rebuildLines();
}

void FixtureManager::setItemIndex(Fixture* item, int newIndex, bool addToUndo)
{
    BaseManager::setItemIndex(item, newIndex, addToUndo);
    PatchSheet::getInstance()->rebuildLines();
}

void FixtureManager::defaultValueChanged(FixtureTypeChannel* ftc)
{
    for (int i = 0; i < items.size(); i++) {
        Fixture* f = items[i];
        for (auto itSf = f->subFixtures.begin(); itSf != f->subFixtures.end(); itSf.next()) {
            SubFixture* sf = itSf.getValue();
            if (sf == nullptr) continue;
            for (auto itSfc = sf->channelsMap.begin(); itSfc != sf->channelsMap.end(); itSfc.next()) {
                SubFixtureChannel *sfc = itSfc.getValue();
                if (sfc->parentFixtureTypeChannel == ftc) {
                    sfc->defaultValue = ftc->defaultValue->floatValue();
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
                    sfc->defaultValue = ftc->defaultValue->floatValue();
                    Brain::getInstance()->pleaseUpdate(sfc);
                }
            }
        }
    }
}



