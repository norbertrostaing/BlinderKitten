/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class Multiplicator :
    public BaseItem
    // public ChainVizTarget
{
public:
    Multiplicator(var params = var());
    virtual ~Multiplicator();

    String objectType;
    var objectData;

    FloatParameter* multValue;

    void onContainerParameterChangedInternal(Parameter* p) override;
    void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) override;

    String getTypeString() const override { return objectType; }
    static Multiplicator* create(var params) { return new Multiplicator(params); }
};

