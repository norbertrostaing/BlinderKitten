#include "Group.h"
#include "GroupManager.h"

/*
  ==============================================================================

    ObjectManager.cpp
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

juce_ImplementSingleton(GroupManager);


GroupManager::GroupManager() :
    BaseManager("Group")
    {
    itemDataType = "Group";
    selectItemWhenCreated = true;
       
}

GroupManager::~GroupManager()
{
    // stopThread(1000);
}


void GroupManager::addItemInternal(Group* o, var data)
{
    // o->addFixtureParamTypeListener(this);
    // if (!isCurrentlyLoadingData) o->globalID->setValue(getFirstAvailableObjectID(o));
}

void GroupManager::removeItemInternal(Group* o)
{
    // o->removeObjectListener(this);
}


void GroupManager::onContainerParameterChanged(Parameter* p)
{
   // if (p == lockUI) for (auto& i : items) i->isUILocked->setValue(lockUI->boolValue());
}

