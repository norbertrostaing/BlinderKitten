/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "TimingPreset.h"

class TimingPresetManager :
    public BaseManager<TimingPreset>
{
public:
    juce_DeclareSingleton(TimingPresetManager, true);

    TimingPresetManager();
    ~TimingPresetManager();

    void addItemInternal(TimingPreset* o, var data) override;
    void removeItemInternal(TimingPreset* o) override;

    void onContainerParameterChanged(Parameter* p) override;

};