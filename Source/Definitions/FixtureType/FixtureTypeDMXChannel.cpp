/*
  ==============================================================================

    FixtureTypeDMXChannel.cpp
    Created: 1 Oct 2025
    Author:  j-mutter

  ==============================================================================
*/

#include "FixtureTypeDMXChannel.h"
#include "FixtureTypeDMXChannelManager.h"
#include "Fixture/FixtureManager.h"

FixtureTypeDMXChannel::FixtureTypeDMXChannel(var params) :
    BaseItem(params.getProperty("name", "DMX Channel")),
    objectType(params.getProperty("type", "FixtureTypeDMXChannel").toString()),
    objectData(params),
    chansManager()
{
    itemDataType = "FixtureTypeDMXChannel";

    dmxDelta = addIntParameter("Channel", "DMX channel offset from base fixture address (1-512)", 1, 1, 512);
    dmxDelta -> setEnabled(false);
    
    resolution = addEnumParameter("Resolution", "");
    resolution->addOption("8bits", "8bits");
    resolution->addOption("16bits", "16bits");
    resolution->addOption("Fine channel only", "fine");
    
    defaultValue = addFloatParameter("Default value", "Default value of the channel (0-255)", 0, 0, 1);
    defaultValue->canBeDisabledByUser = false;

    chansManager.addItem();
    
    addChildControllableContainer(&chansManager);
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

    if (p == defaultValue) { FixtureManager::getInstance()->defaultValueChanged(this); }
}
