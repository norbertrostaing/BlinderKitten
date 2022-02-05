#include "Fixture.h"
#include "FixtureManager.h"

juce_ImplementSingleton(FixtureManager);


FixtureManager::FixtureManager() :
    BaseManager("Fixture")
{
    itemDataType = "Fixture";
    selectItemWhenCreated = true;
       
}

FixtureManager::~FixtureManager()
{
    // stopThread(1000);
}


void FixtureManager::addItemInternal(Fixture* o, var data)
{
    // o->addObjectListener(this);
    // if (!isCurrentlyLoadingData) o->globalID->setValue(getFirstAvailableObjectID(o));
}

void FixtureManager::removeItemInternal(Fixture* o)
{
    // o->removeObjectListener(this);
}


void FixtureManager::onContainerParameterChanged(Parameter* p)
{
   // if (p == lockUI) for (auto& i : items) i->isUILocked->setValue(lockUI->boolValue());
}
