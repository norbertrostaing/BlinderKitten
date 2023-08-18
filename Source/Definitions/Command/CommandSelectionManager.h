/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "CommandSelection.h"
//#include "../SubFixture/SubFixture.h"
class SubFixture;
class ChannelType;

class CommandSelectionManager :
    public BaseManager<CommandSelection>
{
public:
    CommandSelectionManager();
    ~CommandSelectionManager();
    CriticalSection computing;
    Array<SubFixture*> computedSelectedSubFixtures;
    HashMap<SubFixture*, float> subFixtureToPosition;
    void computeSelection();
    void computeSelection(Array<int> groupHistory);
    Array<ChannelType *> getControllableChannelsTypes();

};