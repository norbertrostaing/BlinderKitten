/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "Task.h"

class TaskManager :
    public BaseManager<Task>
{
public:
    TaskManager(String name);
    ~TaskManager();

    void addItemInternal(Task* o, var data) override;
    void removeItemInternal(Task* o) override;

    void onContainerParameterChanged(Parameter* p) override;

};