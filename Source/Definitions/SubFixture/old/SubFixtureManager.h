/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "SubFixture.h"

class SubFixtureManager :
    public BaseManager<SubFixture>
{
public:
    juce_DeclareSingleton(SubFixtureManager, true);

    SubFixtureManager();
    ~SubFixtureManager();

    void addItemInternal(SubFixture* o, var data) override;
    void removeItemInternal(SubFixture* o) override;

    void onContainerParameterChanged(Parameter* p) override;

};