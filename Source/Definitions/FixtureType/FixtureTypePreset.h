/*
  ==============================================================================

    ChannelType.h
    Created: 7 Nov 2021 7:40:48pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Definitions/FixtureType/FixtureTypePresetValues.h"

class FixtureTypePreset:
    public BaseItem
{
    public:
    FixtureTypePreset(var params = var());
    ~FixtureTypePreset();

    String objectType;
    var objectData;

    Trigger* importFromPresetGridBtn;
    Trigger* exportInPresetGridBtn;

    void importFromPresetGrid();
    void exportInPresetGrid(bool addIfNotThere);

    BaseManager<FixtureTypePresetValues> values;
    void onContainerParameterChangedInternal(Parameter* p) override;
    void triggerTriggered(Trigger* t) override;

};


