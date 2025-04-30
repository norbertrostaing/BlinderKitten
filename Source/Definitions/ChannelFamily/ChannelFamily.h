/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "ChannelType/ChannelType.h"

class ChannelFamily :
    public BaseItem
{
public:
    ChannelFamily(var params = var());
    virtual ~ChannelFamily();

    String objectType;
    var objectData;

    BaseManager<ChannelType> definitions;

    void onContainerParameterChangedInternal(Parameter* p) override;
    void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) override;

    String getTypeString() const override { return objectType; }
    static ChannelFamily* create(var params) { return new ChannelFamily(params); }
};

