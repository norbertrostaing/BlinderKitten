/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "CurvePreset.h"

class CurvePresetManager :
    public BaseManager<CurvePreset>
{
public:
    juce_DeclareSingleton(CurvePresetManager, true);

    CurvePresetManager();
    ~CurvePresetManager();

    void addItemInternal(CurvePreset* o, var data) override;
    void removeItemInternal(CurvePreset* o) override;

    void onContainerParameterChanged(Parameter* p) override;

};