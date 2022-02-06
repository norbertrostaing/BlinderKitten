/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "Programmer.h"

class ProgrammerManager :
    public BaseManager<Programmer>
{
public:
    juce_DeclareSingleton(ProgrammerManager, true);

    ProgrammerManager();
    ~ProgrammerManager();

    void addItemInternal(Programmer* o, var data) override;
    void removeItemInternal(Programmer* o) override;

    void onContainerParameterChanged(Parameter* p) override;

};