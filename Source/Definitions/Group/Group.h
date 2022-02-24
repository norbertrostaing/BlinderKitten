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

class Group :
    public BaseItem
{
public:
    Group(var params = var());
    virtual ~Group();

    String objectType;
    var objectData;

    IntParameter* id;
    int registeredId = 0;
    StringParameter* userName;
    void onContainerParameterChangedInternal(Parameter* p);
    void updateName();

    CommandSelectionManager selection;

    String getTypeString() const override { return objectType; }

    static Group* create(var params) { return new Group(params); }
};
