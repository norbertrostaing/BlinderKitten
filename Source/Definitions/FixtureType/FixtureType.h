/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "FixtureTypeChannel.h"
#include "FixtureTypeChannelManager.h"
#include "FixtureTypeVirtualChannelManager.h"
#include "FixtureTypeDMXChannelManager.h"

class FixtureType:
    public BaseItem
{
public:
    FixtureType(var params = var());
    virtual ~FixtureType();

    String objectType;
    var objectData;
    
    ColorParameter* layoutStrokeColor;
    ColorParameter* layoutFillColor;
    FileParameter* layoutIconParameter;

    bool useLayoutIcon = false;
    Image layoutIconImage;
    Image layoutBorderImage;
    Image layoutContentImage;

    IntParameter* templateId;
    IntParameter* copyToId;
    Trigger* copyTemplateButton;

    FixtureTypeDMXChannelManager dmxChannelsManager;
    FixtureTypeVirtualChannelManager virtualChansManager;
    ControllableContainer helpContainer;

    String getTypeString() const override { return objectType; }

    static FixtureType* create(var params) { return new FixtureType(params); }
    void afterLoadJSONDataInternal();
    void updateVirtualLists();

    void onContainerParameterChangedInternal(Parameter* p);
    void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c);
    void copyTemplate();
};

