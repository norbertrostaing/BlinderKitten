/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
//#include "../SubFixture/SubFixture.h"
#include "FixtureTypeVirtualChannel.h"
class SubFixture;

class FixtureTypeVirtualChannelManager :
    public BaseManager<FixtureTypeVirtualChannel>
{
public:
    FixtureTypeVirtualChannelManager();
    ~FixtureTypeVirtualChannelManager();
    void addItemInternal(FixtureTypeVirtualChannel* c, var data);

};