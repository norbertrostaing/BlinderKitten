/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

// #include "../Command/CommandSelectionManager.h"
class CommandSelectionManager;

class TimingPreset :
    public BaseItem
{
public:
    TimingPreset(var params = var());
    virtual ~TimingPreset();

    String objectType;
    var objectData;

    IntParameter* id;
    CommandSelectionManager* selection;

    String getTypeString() const override { return objectType; }

    void parameterValueChanged(Parameter* p);

    static TimingPreset* create(var params) { return new TimingPreset(params); }
};
