/*
  ==============================================================================

    DeviceTypeChannel.cpp
    Created: 8 Nov 2021 7:28:28pm
    Author:  No

  ==============================================================================
*/

#include "DeviceTypeChannel.h"
#include "../FixtureParamType/FixtureParamTypeManager.h"


DeviceTypeChannel::DeviceTypeChannel(var params) :
    BaseItem(params.getProperty("name", "Channel")),
    objectType(params.getProperty("type", "DeviceTypeChannel").toString()),
    objectData(params)

{
    nameCanBeChangedByUser = false;
    channelType = addTargetParameter("Channel type", "Type of data of this channel", FixtureParamTypeManager::getInstance());
    channelType -> targetType = TargetParameter::CONTAINER;
    channelType -> maxDefaultSearchLevel = 2;

    resolution = addEnumParameter("Resolution", "");
    resolution->addOption("8bits", "8bits");
    resolution->addOption("16bits", "16bits");

    suffix = addStringParameter("Fixture Suffix", "This suffix is added to the name of the device to create the name of the fixture", "");
    idGroupName = addStringParameter("ID Group name", "All fixtures with the same id group name have following fixtures id", "");
    defaultValue = addFloatParameter("Default value", "Default value of the channel", 0, 0, 1);
    dmxDelta = addIntParameter("DMX Channel", "Number of the channel in the DMX chart", 1, 1);
    dmxDelta -> setEnabled(false);
    
};

DeviceTypeChannel::~DeviceTypeChannel()
{
};

