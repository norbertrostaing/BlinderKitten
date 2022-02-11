#include "SubFixture.h"
#include "SubFixtureManager.h"

juce_ImplementSingleton(SubFixtureManager);


SubFixtureManager::SubFixtureManager() :
    BaseManager("SubFixture")
{
    itemDataType = "SubFixture";
    selectItemWhenCreated = true;
       
}

SubFixtureManager::~SubFixtureManager()
{
    // stopThread(1000);
}


void SubFixtureManager::addItemInternal(SubFixture* o, var data)
{
    // o->addObjectListener(this);
    // if (!isCurrentlyLoadingData) o->globalID->setValue(getFirstAvailableObjectID(o));
}

void SubFixtureManager::removeItemInternal(SubFixture* o)
{
    // o->removeObjectListener(this);
}


void SubFixtureManager::onContainerParameterChanged(Parameter* p)
{
   // if (p == lockUI) for (auto& i : items) i->isUILocked->setValue(lockUI->boolValue());
}
