/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "Bundle.h"

class BundleManager :
    public BaseManager<Bundle>
{
public:
    juce_DeclareSingleton(BundleManager, true);

    BundleManager();
    ~BundleManager();

    void addItemInternal(Bundle* o, var data) override;
    void removeItemInternal(Bundle* o) override;

    void onContainerParameterChanged(Parameter* p) override;

};