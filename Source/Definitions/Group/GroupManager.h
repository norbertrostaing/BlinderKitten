/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "Group.h"

class GroupManager :
    public BaseManager<Group>
{
public:
    juce_DeclareSingleton(GroupManager, true);

    GroupManager();
    ~GroupManager();

    void addItemInternal(Group* o, var data) override;
    void removeItemInternal(Group* o) override;

    void onContainerParameterChanged(Parameter* p) override;

};