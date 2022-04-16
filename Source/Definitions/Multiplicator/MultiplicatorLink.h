/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class MultiplicatorLink :
    public BaseItem
    // public ChainVizTarget
{
public:
    MultiplicatorLink(var params = var());
    virtual ~MultiplicatorLink();

    String objectType;
    var objectData;

    TargetParameter* targetMult;

    void onContainerParameterChangedInternal(Parameter* p) override;
    void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) override;

    String getTypeString() const override { return objectType; }
    static MultiplicatorLink* create(var params) { return new MultiplicatorLink(params); }
};

