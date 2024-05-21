/*
  ==============================================================================

    ChannelType.h
    Created: 7 Nov 2021 7:40:48pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "../Command/CommandSelectionManager.h"
#include "MapperStep.h"

class MapperRow:
    public BaseItem
{
    public:
    MapperRow(var params = var());
    ~MapperRow();

    String objectType;
    var objectData;

    BaseManager<MapperStep> paramContainer;

    TargetParameter* followedChannel;

    CommandSelectionManager selection;

    HashMap<SubFixture*, double> computedPositions;
    HashMap<SubFixtureChannel*,double> subFixtureChannelOffsets;
    CriticalSection isComputing;

    void computeData();

    void onControllableFeedbackUpdate(ControllableContainer*, Controllable*);
    void updateDisplay();


};