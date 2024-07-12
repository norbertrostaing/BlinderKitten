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
#include "FixtureTypeVirtualChannelManager.h"
#include "FixtureType.h"
#include "Tracker/TrackerManager.h"
#include "Fixture/FixtureManager.h"

FixtureTypeChannel::FixtureTypeChannel(var params) :
    BaseItem(params.getProperty("name", "Channel")),
    objectType(params.getProperty("type", "FixtureTypeChannel").toString()),
    objectData(params)

{
    nameCanBeChangedByUser = false;
    canBeDisabled = false;
    channelType = addTargetParameter("Channel type", "Type of data of this channel", ChannelFamilyManager::getInstance());
    channelType -> targetType = TargetParameter::CONTAINER;
    channelType -> maxDefaultSearchLevel = 2;

    resolution = addEnumParameter("Resolution", "");
    resolution->addOption("8bits", "8bits");
    resolution->addOption("16bits", "16bits");

    subFixtureId = addIntParameter("SubFixture ID", "0 means not in a subfixture",0,0);
    defaultValue = addFloatParameter("Default value", "Default value of the channel", 0, 0, 1);
    highlightValue = addFloatParameter("Highlight value", "Value of the channel during highlight", 0, 0, 1);
    killedBySWOP = addBoolParameter("Killed By SWOP", "if checked, this parameter will be set to its default value when cuelists with no command for ot are called with SWOP", false);

    fadeOrSnap = addEnumParameter("Fade or snap", "Is ths channel allowed to fade or should it jump to his new value ?");
    fadeOrSnap->addOption("Fade", "fade");
    fadeOrSnap->addOption("Snap", "snap");

    invertOutput = addBoolParameter("Invert output", "if checked, output will be inverted", false);

    dmxDelta = addIntParameter("DMX Channel", "Number of the channel in the DMX chart", 1, 1);
    dmxDelta -> setEnabled(false);

    FixtureType* ft = dynamic_cast<FixtureType*>(parentContainer.get());
    virtualMaster = addTargetParameter("Virtual Master", "Select a virtual master");
    virtualMaster->targetType = TargetParameter::CONTAINER;
    virtualMaster->maxDefaultSearchLevel = 2;

    physicalRange = addPoint2DParameter("Physical range", "Range output (degrees for pan, tilt, zoom etc...");
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
    else if (p == physicalRange) {
        TrackerManager::getInstance()->recomputeAllTrackers();
    }
    else if (p == defaultValue || p == invertOutput) {
        FixtureManager::getInstance()->defaultValueChanged(this);
    }
}
