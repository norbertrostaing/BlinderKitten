#include "Task.h"
#include "TaskManager.h"

/*
  ==============================================================================

    ObjectManager.cpp
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

TaskManager::TaskManager(String name) :
    BaseManager(name)    {
    itemDataType = "Task";
    selectItemWhenCreated = false;
       
}

TaskManager::~TaskManager()
{
}


void TaskManager::addItemInternal(Task* o, var data)
{
    // o->addTaskListener(this);
    // if (!isCurrentlyLoadingData) o->globalID->setValue(getFirstAvailableObjectID(o));
}

void TaskManager::removeItemInternal(Task* o)
{
    // o->removeObjectListener(this);
}


void TaskManager::onContainerParameterChanged(Parameter* p)
{
   // if (p == lockUI) for (auto& i : items) i->isUILocked->setValue(lockUI->boolValue());
}
