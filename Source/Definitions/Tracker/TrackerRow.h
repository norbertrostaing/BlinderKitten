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
#include "TrackerStep.h"

class TrackerRow:
    public BaseItem
{
    public:
    TrackerRow(var params = var());
    ~TrackerRow();

    String objectType;
    var objectData;

    BaseManager<TrackerStep> paramContainer;

    TargetParameter* followedChannel;

    CommandSelectionManager selection;

    HashMap<SubFixture*, double> computedPositions;
    HashMap<SubFixtureChannel*,double> subFixtureChannelOffsets;

    void computeData();

    void onControllableFeedbackUpdate(ControllableContainer*, Controllable*);
    void updateDisplay();


};