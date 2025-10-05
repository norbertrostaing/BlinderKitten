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

    highlightValue = addFloatParameter("Highlight value", "Value of the channel during highlight", 0, 0, 1);
    highlightValue->canBeDisabledByUser = true;
    highlightValue->setEnabled(false);

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

        chansManager.updateCanAddItems();

        // Show warning if trying to set non-8bit resolution with multiple channels
        if (chansManager.items.size() > 1 && p->getValue() != "8bits") {
            juce::AlertWindow::showMessageBox (
                juce::AlertWindow::WarningIcon,
                "Cannot change resolution",
                "Only 8-bit channels can be split into multiple logical channels. Please remove extra logical channels first.",
                "Ok",
                &ShapeShifterManager::getInstance()->mainContainer);
            p->setValue("8bits");
        }
    }

    if (p == defaultValue) { FixtureManager::getInstance()->defaultValueChanged(this); }
}
