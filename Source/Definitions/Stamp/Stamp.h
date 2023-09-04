/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "../Command/CommandSelectionManager.h"

class Stamp :
    public BaseItem
{
public:
    Stamp(var params = var());
    virtual ~Stamp();

    String objectType;
    var objectData;

    IntParameter* id;
    int registeredId = 0;
    StringParameter* userName;
    void onContainerParameterChangedInternal(Parameter* p);
    void updateName();
    void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c);

    CommandSelectionManager selection;

    String getTypeString() const override { return objectType; }

    static Stamp* create(var params) { return new Stamp(params); }
};
