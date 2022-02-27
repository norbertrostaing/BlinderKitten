/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "VirtualFaderCol.h"

class VirtualFaderColManager :
    public BaseManager<VirtualFaderCol>
{
public:
    juce_DeclareSingleton(VirtualFaderColManager, true);

    VirtualFaderColManager();
    ~VirtualFaderColManager();

    void addItemInternal(VirtualFaderCol* o, var data) override;
    void removeItemInternal(VirtualFaderCol* o) override;

    void onContainerParameterChanged(Parameter* p) override;

    // static int sort(VirtualFaderCol* A, VirtualFaderCol* B) {return (int)A->id->getValue() - (int)B->id->getValue();};

};