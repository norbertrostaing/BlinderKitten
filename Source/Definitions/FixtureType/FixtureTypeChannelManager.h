/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
//#include "../SubFixture/SubFixture.h"
#include "FixtureTypeChannel.h"
class SubFixture;

class FixtureTypeChannelManager :
    public BaseManager<FixtureTypeChannel>
{
public:
    FixtureTypeChannelManager();
    ~FixtureTypeChannelManager();

    void calcDmxChannels();

    String parentDMXChannelResoltion();
    void updateCanAddItems();

    void addItemInternal(FixtureTypeChannel* c, var data) override;
    void askForRemoveBaseItem(BaseItem* item);
    void askForDuplicateItem(BaseItem* item);
    void askForPaste();
    void askForMoveBefore(BaseItem* i);
    void askForMoveAfter(BaseItem* i);
    void setItemIndex(FixtureTypeChannel* item, int newIndex, bool addToUndo = true) override;

};
