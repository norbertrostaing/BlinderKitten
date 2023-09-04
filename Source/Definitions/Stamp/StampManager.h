/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "Stamp.h"

class StampManager :
    public BaseManager<Stamp>
{
public:
    juce_DeclareSingleton(StampManager, true);

    StampManager();
    ~StampManager();

    void addItemInternal(Stamp* o, var data) override;
    void removeItemInternal(Stamp* o) override;

    void onContainerParameterChanged(Parameter* p) override;

    // static int sort(Stamp* A, Stamp* B) {return (int)A->id->getValue() - (int)B->id->getValue();};

};