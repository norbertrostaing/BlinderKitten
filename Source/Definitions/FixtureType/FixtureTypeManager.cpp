#include "FixtureType.h"
#include "FixtureTypeManager.h"

/*
  ==============================================================================

    ObjectManager.cpp
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

juce_ImplementSingleton(FixtureTypeManager);

#include "Brain.h"
#include "UserInputManager.h"


FixtureTypeManager::FixtureTypeManager() :
    BaseManager("FixtureType")
    {
    itemDataType = "FixtureType";
    selectItemWhenCreated = true;
       
}

FixtureTypeManager::~FixtureTypeManager()
{
    // stopThread(1000);
}


void FixtureTypeManager::addItemInternal(FixtureType* o, var data)
{
    // o->addChannelFamilyListener(this);
    // if (!isCurrentlyLoadingData) o->globalID->setValue(getFirstAvailableObjectID(o));
}

void FixtureTypeManager::removeItemInternal(FixtureType* o)
{
    if (!Brain::getInstance()->isClearing && !Brain::getInstance()->loadingIsRunning) {
        UserInputManager::getInstance()->processInput("ClearAll");
    }
    // o->removeObjectListener(this);
}


void FixtureTypeManager::onContainerParameterChanged(Parameter* p)
{
   // if (p == lockUI) for (auto& i : items) i->isUILocked->setValue(lockUI->boolValue());
}

