/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "Effect.h"

class EffectManager :
    public BaseManager<Effect>
{
public:
    juce_DeclareSingleton(EffectManager, true);

    EffectManager();
    ~EffectManager();

    void addItemInternal(Effect* o, var data) override;
    void removeItemInternal(Effect* o) override;

    void onContainerParameterChanged(Parameter* p) override;

};