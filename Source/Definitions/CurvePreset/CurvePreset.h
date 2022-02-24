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

class CurvePreset :
    public BaseItem
{
public:
    CurvePreset(var params = var());
    virtual ~CurvePreset();

    String objectType;
    var objectData;

    IntParameter* id;
    int registeredId = 0;
    StringParameter* userName;
    void onContainerParameterChangedInternal(Parameter* p);
    void updateName();
    Automation curve;

    String getTypeString() const override { return objectType; }

    static CurvePreset* create(var params) { return new CurvePreset(params); }
};
