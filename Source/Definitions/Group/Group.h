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

class Group :
    public BaseItem
{
public:
    Group(var params = var());
    virtual ~Group();

    String objectType;
    var objectData;

    IntParameter* id;
    CommandSelectionManager* selection;

    String getTypeString() const override { return objectType; }

    void parameterValueChanged(Parameter* p);

    static Group* create(var params) { return new Group(params); }
};
