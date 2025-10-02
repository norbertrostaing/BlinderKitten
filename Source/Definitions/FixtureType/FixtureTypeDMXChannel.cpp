/*
  ==============================================================================

    FixtureTypeDMXChannel.cpp
    Created: 1 Oct 2025
    Author:  j-mutter

  ==============================================================================
*/

#include "FixtureTypeDMXChannel.h"
#include "FixtureTypeDMXChannelManager.h"

FixtureTypeDMXChannel::FixtureTypeDMXChannel(var params) :
    BaseItem(params.getProperty("name", "DMX Channel")),
    objectType(params.getProperty("type", "FixtureTypeDMXChannel").toString()),
    objectData(params)
{
    itemDataType = "FixtureTypeDMXChannel";

    dmxDelta = addIntParameter("Channel", "DMX channel offset from base fixture address (1-512)", 1, 1, 512);
    dmxDelta -> setEnabled(false);
    
    resolution = addEnumParameter("Resolution", "");
    resolution->addOption("8bits", "8bits");
    resolution->addOption("16bits", "16bits");
    resolution->addOption("Fine channel only", "fine");
    
    highlightValue = addFloatParameter("Highlight Value", "Value to use when highlighting this channel", 0, 0, 1);
    highlightValue->canBeDisabledByUser = true;
    highlightValue->setEnabled(false);
}

FixtureTypeDMXChannel::~FixtureTypeDMXChannel()
{
}

void FixtureTypeDMXChannel::onContainerParameterChangedInternal(Parameter* p)
{
    if (p == resolution) {
        FixtureTypeDMXChannelManager* manager = dynamic_cast<FixtureTypeDMXChannelManager*>(parentContainer.get());
        if (manager != nullptr) {
            manager->calcDmxChannels();
        }
    }
//    else if (p == physicalRange) {
//        TrackerManager::getInstance()->recomputeAllTrackers();
//    }

    else if (p == highlightValue) {
        if (!p->enabled) p->setEnabled(true);
    }
}
