/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "../Command/CommandValueManager.h"

class Stamp :
    public ChangeBroadcaster,
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

    IntParameter* layoutId;
    IntParameter* groupId;
    Point2DParameter* position;
    Point2DParameter* size;
    FloatParameter* angle;

    ControllableContainer stampValues;
    CommandValueManager redFull;
    CommandValueManager redZero;
    CommandValueManager greenFull;
    CommandValueManager greenZero;
    CommandValueManager blueFull;
    CommandValueManager blueZero;

    void onContainerParameterChangedInternal(Parameter* p);
    void updateName();
    void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c);

    String getTypeString() const override { return objectType; }
    static Stamp* create(var params) { return new Stamp(params); }
};
