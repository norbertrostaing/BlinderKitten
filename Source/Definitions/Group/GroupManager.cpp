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

int compare(Group* A, Group* B) {
    return (int)A->id->getValue() - (int)B->id->getValue();
}

GroupManager::GroupManager() :
    BaseManager("Group")
{
    itemDataType = "Group";
    selectItemWhenCreated = true;
    comparator.compareFunc = compare;
    autoReorderOnAdd = true;
}

GroupManager::~GroupManager()
{
    // stopThread(1000);
}


void GroupManager::addItemInternal(Group* o, var data)
{
    reorderItems();
}

void GroupManager::removeItemInternal(Group* o)
{
}

void GroupManager::onContainerParameterChanged(Parameter* p)
{
}

