#include "FixtureType.h"
#include "FixtureTypeManager.h"

/*
  ==============================================================================

    ObjectManager.cpp
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

juce_ImplementSingleton(FixtureTypeManager);

#include "Brain.h"
#include "UserInputManager.h"


FixtureTypeManager::FixtureTypeManager() :
    BaseManager("FixtureType")
    {
    itemDataType = "FixtureType";
    selectItemWhenCreated = true;
       
}

FixtureTypeManager::~FixtureTypeManager()
{
    // stopThread(1000);
}


juce::Array<FixtureType *> FixtureTypeManager::addItemsFromData(juce::var data, bool addToUndo) {
    if (!data.isArray()) {
        data = var(Array(data));
    }
        
    // Check if this is an old format file that has a "channels" as the top most container level instead of "dmxChannels"
    // If yes, we need to modify the structure to match the new one.
    for (int n = 0; n < data.size(); n++) {
        var oldData = data[n];
        var containers = oldData.getProperty("containers", var());
        var oldChannelsData = containers.getProperty("channels", var());

        if (oldChannelsData.isObject()) {
            LOG("Migrating old FixtureType format to new DMX channel structure");

            var newDmxChannelsContainer = var(new DynamicObject());
            Array<var> newDmxChannelItems;

            if (oldChannelsData.hasProperty("items")) {
                var oldItems = oldChannelsData.getProperty("items", var());

                if (oldItems.isArray()) {
                    for (int i = 0; i < oldItems.size(); i++) {
                        var oldChannelData = oldItems[i];
                        var oldParameters = oldChannelData.getProperty("parameters", var());

                        // Create a DMX channel item
                        var newDmxChannelItem = var(new DynamicObject());
                        newDmxChannelItem.getDynamicObject()->setProperty("niceName", oldChannelData.getProperty("niceName", "Channel"));
                        newDmxChannelItem.getDynamicObject()->setProperty("type", "FixtureTypeDMXChannel");

                        // Create logical channel item (copy from old channel but remove DMX-level params)
                        var newLogicalChannelItem = oldChannelData.clone();
                        newLogicalChannelItem.getDynamicObject()->setProperty("type", "FixtureTypeChannel");

                        std::vector<juce::String> dmxChannelParameterNames { "/dmxChannel", "/resolution", "/defaultValue", "/highlightValue" };

                        if (oldParameters.isArray()) {
                            Array<var> dmxChannelParams;
                            Array<var> logicalChannelParams;

                            for (int j = 0; j < oldParameters.size(); j++) {
                                var param = oldParameters[j];
                                String controlAddress = param.getProperty("controlAddress", String(""));

                                bool found = std::find(dmxChannelParameterNames.begin(), dmxChannelParameterNames.end(), controlAddress) != dmxChannelParameterNames.end();
                                if (found) {
                                    // This parameter goes to the DMX channel level
                                    if (controlAddress == "/dmxChannel") {
                                        param.getDynamicObject()->setProperty("controlAddress", "/channel");
                                    }
                                    dmxChannelParams.add(param);
                                } else {
                                    // This parameter stays at the logical channel level
                                    logicalChannelParams.add(param);
                                }
                            }

                            // Set parameters on DMX channel
                            newDmxChannelItem.getDynamicObject()->setProperty("parameters", var(dmxChannelParams));
                            newLogicalChannelItem.getDynamicObject()->setProperty("parameters", var(logicalChannelParams));

                            // Create the nested structure: dmxChannel -> containers -> channels -> items
                            var logicalChannelsContainer = var(new DynamicObject());
                            logicalChannelsContainer.getDynamicObject()->setProperty("items", Array<var>{newLogicalChannelItem});

                            var dmxChannelContainers = var(new DynamicObject());
                            dmxChannelContainers.getDynamicObject()->setProperty("channels", logicalChannelsContainer);

                            newDmxChannelItem.getDynamicObject()->setProperty("containers", dmxChannelContainers);
                        }

                        newDmxChannelItems.add(newDmxChannelItem);
                    }
                }
            }

            newDmxChannelsContainer.getDynamicObject()->setProperty("items", newDmxChannelItems);

            containers.getDynamicObject()->removeProperty("channels");
            containers.getDynamicObject()->setProperty("dmxChannels", newDmxChannelsContainer);
        }
    }
    
    return BaseManager::addItemsFromData(data, addToUndo);
}

void FixtureTypeManager::addItemInternal(FixtureType* o, var data)
{
    // o->addChannelFamilyListener(this);
    // if (!isCurrentlyLoadingData) o->globalID->setValue(getFirstAvailableObjectID(o));
}

void FixtureTypeManager::removeItemInternal(FixtureType* o)
{
    if (!Brain::getInstance()->isClearing && !Brain::getInstance()->loadingIsRunning) {
        UserInputManager::getInstance()->processInput("ClearAll");
    }
    // o->removeObjectListener(this);
}


void FixtureTypeManager::onContainerParameterChanged(Parameter* p)
{
   // if (p == lockUI) for (auto& i : items) i->isUILocked->setValue(lockUI->boolValue());
}

