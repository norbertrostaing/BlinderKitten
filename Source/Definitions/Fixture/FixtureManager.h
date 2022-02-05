/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "Fixture.h"

class FixtureManager :
    public BaseManager<Fixture>
{
public:
    juce_DeclareSingleton(FixtureManager, true);

    FixtureManager();
    ~FixtureManager();

    void addItemInternal(Fixture* o, var data) override;
    void removeItemInternal(Fixture* o) override;

    void onContainerParameterChanged(Parameter* p) override;

};