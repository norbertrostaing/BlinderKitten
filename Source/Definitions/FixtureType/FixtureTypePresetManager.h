/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
//#include "../SubFixture/SubFixture.h"
#include "FixtureTypePreset.h"
class SubFixture;

class FixtureTypePresetManager :
    public BaseManager<FixtureTypePreset>
{
public:
    FixtureTypePresetManager();
    ~FixtureTypePresetManager();
    void addItemInternal(FixtureTypePreset* c, var data);

};