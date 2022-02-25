/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "VirtualButton.h"

class VirtualButtonManager :
    public BaseManager<VirtualButton>
{
public:
    juce_DeclareSingleton(VirtualButtonManager, true);

    VirtualButtonManager();
    ~VirtualButtonManager();

    void addItemInternal(VirtualButton* o, var data) override;
    void removeItemInternal(VirtualButton* o) override;

    void onContainerParameterChanged(Parameter* p) override;

    // static int sort(VirtualButton* A, VirtualButton* B) {return (int)A->id->getValue() - (int)B->id->getValue();};

};