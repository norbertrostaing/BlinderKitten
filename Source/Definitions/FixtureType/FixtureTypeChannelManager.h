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

    bool massiveImport = false;

    void calcDmxChannels();
    void addItemInternal(FixtureTypeChannel* c, var data) override;
    void askForRemoveBaseItem(BaseItem* item) override;
    void askForDuplicateItem(BaseItem* item) override;
    void askForPaste() override;
    void askForMoveBefore(BaseItem* i) override;
    void askForMoveAfter(BaseItem* i) override;
    void setItemIndex(FixtureTypeChannel* item, int newIndex, bool addToUndo = true) override;

};
