/*
  ==============================================================================

    FixtureTypeChannel.cpp
    Created: 8 Nov 2021 7:28:28pm
    Author:  No

  ==============================================================================
*/

#include "FixtureTypeChannel.h"
#include "../ChannelFamily/ChannelFamilyManager.h"
#include "FixtureTypeChannelManager.h"


FixtureTypeChannel::FixtureTypeChannel(var params) :
    BaseItem(params.getProperty("name", "Channel")),
    objectType(params.getProperty("type", "FixtureTypeChannel").toString()),
    objectData(params)

{
    nameCanBeChangedByUser = false;
    channelType = addTargetParameter("Channel type", "Type of data of this channel", ChannelFamilyManager::getInstance());
    channelType -> targetType = TargetParameter::CONTAINER;
    channelType -> maxDefaultSearchLevel = 2;

    resolution = addEnumParameter("Resolution", "");
    resolution->addOption("8bits", "8bits");
    resolution->addOption("16bits", "16bits");

    subFixtureId = addIntParameter("SubFixture ID", "0 means not in a subfixture",0,0);
    defaultValue = addFloatParameter("Default value", "Default value of the channel", 0, 0, 1);
    killedBySWOP = addBoolParameter("Killed By SWOP", "if checked, this parameter will be set to its default value when cuelists with no command for ot are called with SWOP",false);
    dmxDelta = addIntParameter("DMX Channel", "Number of the channel in the DMX chart", 1, 1);
    dmxDelta -> setEnabled(false);
    
};

FixtureTypeChannel::~FixtureTypeChannel()
{
};

void FixtureTypeChannel::onContainerParameterChangedInternal(Parameter* p) {
    if (p == resolution) {
        FixtureTypeChannelManager* p = dynamic_cast<FixtureTypeChannelManager*>(parentContainer.get());
        if (p != nullptr) {
            p->calcDmxChannels();
        }
    }
}
