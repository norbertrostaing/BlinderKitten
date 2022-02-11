/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "FixtureType.h"

class FixtureTypeManager :
    public BaseManager<FixtureType>
{
public:
    juce_DeclareSingleton(FixtureTypeManager, true);

    FixtureTypeManager();
    ~FixtureTypeManager();

    void addItemInternal(FixtureType* o, var data) override;
    void removeItemInternal(FixtureType* o) override;

    void onContainerParameterChanged(Parameter* p) override;

};