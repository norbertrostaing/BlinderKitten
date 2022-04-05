/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "Tracker.h"

class TrackerManager :
    public BaseManager<Tracker>
{
public:
    juce_DeclareSingleton(TrackerManager, true);

    TrackerManager();
    ~TrackerManager();

    void addItemInternal(Tracker* o, var data) override;
    void removeItemInternal(Tracker* o) override;

    void onContainerParameterChanged(Parameter* p) override;

};