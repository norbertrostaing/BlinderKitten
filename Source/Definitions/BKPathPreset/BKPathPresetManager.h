/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "BKPathPreset.h"

class BKPathPresetManager :
    public BaseManager<BKPathPreset>
{
public:
    juce_DeclareSingleton(BKPathPresetManager, true);

    BKPathPresetManager();
    ~BKPathPresetManager();

    void addItemInternal(BKPathPreset* o, var data) override;
    void removeItemInternal(BKPathPreset* o) override;

    void onContainerParameterChanged(Parameter* p) override;

};