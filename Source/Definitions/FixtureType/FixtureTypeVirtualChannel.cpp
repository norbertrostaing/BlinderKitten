/*
  ==============================================================================

    FixtureTypeVirtualChannel.cpp
    Created: 8 Nov 2021 7:28:28pm
    Author:  No

  ==============================================================================
*/

#include "FixtureTypeVirtualChannel.h"
#include "../ChannelFamily/ChannelFamilyManager.h"
#include "FixtureTypeVirtualChannelManager.h"
#include "Fixture/FixtureManager.h"

FixtureTypeVirtualChannel::FixtureTypeVirtualChannel(var params) :
    BaseItem(params.getProperty("name", "Virt Channel 1")),
    objectType(params.getProperty("type", "FixtureTypeVirtualChannel").toString()),
    objectData(params)

{
    canBeDisabled = false;
    channelType = addTargetParameter("Channel type", "Type of data of this channel", ChannelFamilyManager::getInstance());
    channelType -> targetType = TargetParameter::CONTAINER;
    channelType -> maxDefaultSearchLevel = 2;

    subFixtureId = addIntParameter("SubFixture ID", "0 means not in a subfixture",0,0);
    defaultValue = addFloatParameter("Default value", "Default value of the channel", 0, 0, 1);
    highlightValue = addFloatParameter("Highlight value", "Highlight value", 1, 0, 1);
    killedBySWOP = addBoolParameter("Killed By SWOP", "if checked, this parameter will be set to its default value when cuelists with no command for ot are called with SWOP",false);
    
    fadeOrSnap = addEnumParameter("Fade or snap", "Is ths channel allowed to fade or should it jump to his new value ?");
    fadeOrSnap->addOption("Fade", "fade");
    fadeOrSnap->addOption("Snap", "snap");
};

FixtureTypeVirtualChannel::~FixtureTypeVirtualChannel()
{
};

void FixtureTypeVirtualChannel::onContainerParameterChangedInternal(Parameter* p) {
    if (p == defaultValue) {
        FixtureManager::getInstance()->defaultValueChanged(this);
    }

}
