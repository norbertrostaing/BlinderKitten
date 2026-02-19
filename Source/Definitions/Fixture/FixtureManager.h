/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "Fixture.h"

class FixtureTypeChannel;
class FixtureTypeVirtualChannel;

class FixtureManager :
    public BaseManager<Fixture>
{
public:
    juce_DeclareSingleton(FixtureManager, true);

    FixtureManager();
    ~FixtureManager();

    void addItemInternal(Fixture* o, var data) override;
    void removeItemInternal(Fixture* o) override;
    void askForMoveBefore(BaseItem*) override;
    void askForMoveAfter(BaseItem*) override;
    void askForDuplicateItem(BaseItem* item) override;
    void setItemIndex(Fixture* item, int newIndex, bool addToUndo = true) override;

    void defaultValueChanged(FixtureTypeChannel* ftc);
    void defaultValueChanged(FixtureTypeVirtualChannel* ftc);

    void reorderItems() override;

};
