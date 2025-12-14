/*
  ==============================================================================

    ChannelType.cpp
    Created: 7 Nov 2021 7:40:48pm
    Author:  No

  ==============================================================================
*/

#include "ChannelType.h"

ChannelType::ChannelType(var params) :
    BaseItem(params.getProperty("name", "Channel")),
    objectType(params.getProperty("type", "ChannelType").toString()),
    objectData(params)
{
    priority = addEnumParameter("Channel mode", "Type of channel", true);
    priority -> addOption("Latest Takes Precedence", "LTP");
    priority->addOption("Highest Takes Precedence", "HTP");

    reactGM = addBoolParameter("React to grand master", "If checked, this channel will be dimmed by the grand master", false);
    canBeDisabled = false;
};

ChannelType::~ChannelType()
{
}

