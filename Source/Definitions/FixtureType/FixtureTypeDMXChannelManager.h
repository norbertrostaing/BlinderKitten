/*
  ==============================================================================

    FixtureTypeDMXChannelManager.h
    Created: 1 Oct 2025
    Author:  j-mutter

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "FixtureTypeDMXChannel.h"

class FixtureTypeDMXChannelManager : public BaseManager<FixtureTypeDMXChannel>
{
public:
    FixtureTypeDMXChannelManager();
    ~FixtureTypeDMXChannelManager();

    void calcDmxChannels();
    void addItemInternal(FixtureTypeDMXChannel* c, var data) override;
    void askForRemoveBaseItem(BaseItem* item) override;
    void askForDuplicateItem(BaseItem* item) override;
    void askForPaste() override;
    void askForMoveBefore(BaseItem* i) override;
    void askForMoveAfter(BaseItem* i) override;
    void setItemIndex(FixtureTypeDMXChannel* item, int newIndex, bool addToUndo = true) override;
};
