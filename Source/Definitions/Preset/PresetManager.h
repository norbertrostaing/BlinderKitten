/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "Preset.h"

class PresetManager :
    public BaseManager<Preset>
{
public:
    juce_DeclareSingleton(PresetManager, true);

    PresetManager();
    ~PresetManager();

    void addItemInternal(Preset* o, var data) override;
    void removeItemInternal(Preset* o) override;

};