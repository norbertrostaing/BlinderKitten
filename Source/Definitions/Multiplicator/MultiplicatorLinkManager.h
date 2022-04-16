/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "MultiplicatorLink.h"

class MultiplicatorLinkManager :
    public BaseManager<MultiplicatorLink>
{
public:
    MultiplicatorLinkManager(String name);
    ~MultiplicatorLinkManager();

    void addItemInternal(MultiplicatorLink* o, var data) override;
    void removeItemInternal(MultiplicatorLink* o) override;

    void onContainerParameterChanged(Parameter* p) override;

    float getValue();

};