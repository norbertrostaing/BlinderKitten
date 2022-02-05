/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "Cuelist.h"

class CuelistManager :
    public BaseManager<Cuelist>
{
public:
    juce_DeclareSingleton(CuelistManager, true);

    CuelistManager();
    ~CuelistManager();

    void addItemInternal(Cuelist* o, var data) override;
    void removeItemInternal(Cuelist* o) override;

    void onContainerParameterChanged(Parameter* p) override;

};