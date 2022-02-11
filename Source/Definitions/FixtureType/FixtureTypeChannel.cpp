/*
  ==============================================================================

    FixtureTypeChannel.cpp
    Created: 8 Nov 2021 7:28:28pm
    Author:  No

  ==============================================================================
*/

#include "FixtureTypeChannel.h"
#include "../ChannelFamily/ChannelFamilyManager.h"


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

    // suffix = addStringParameter("SubFixture Suffix", "This suffix is added to the name of the Fixture to create the name of the SubFixture", "");
    // idGroupName = addStringParameter("ID Group name", "All SubFixtures with the same id group name have following SubFixtures id", "");
    subFixtureId = addIntParameter("SubFixture ID", "0 means not in a subfixture",0,0);
    defaultValue = addFloatParameter("Default value", "Default value of the channel", 0, 0, 1);
    dmxDelta = addIntParameter("DMX Channel", "Number of the channel in the DMX chart", 1, 1);
    dmxDelta -> setEnabled(false);
    
};

FixtureTypeChannel::~FixtureTypeChannel()
{
};

