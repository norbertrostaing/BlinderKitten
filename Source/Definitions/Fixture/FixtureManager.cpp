#include "Fixture.h"
#include "FixtureManager.h"
#include "UI/GridView/FixtureGridView.h"

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

}

void FixtureManager::removeItemInternal(Fixture* o)
{
    // o->removeObjectListener(this);
    FixtureGridView::getInstance()->updateCells();

}


