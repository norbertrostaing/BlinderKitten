/*
  ==============================================================================

    FixtureDMXChannel.cpp
    Created: 1 Oct 2025
    Author:  j-mutter

  ==============================================================================
*/

#include "FixtureDMXChannel.h"
#include "Fixture.h"
#include "../FixtureType/FixtureTypeDMXChannel.h"
#include "../FixtureType/FixtureTypeChannel.h"
#include "../SubFixture/SubFixtureChannel.h"
#include "../Interface/InterfaceManager.h"
#include "../Interface/interfaces/dmx/DMXInterface.h"

FixtureDMXChannel::FixtureDMXChannel(Fixture* parentFixture, FixtureTypeDMXChannel* typeChannel) :
    parentFixture(parentFixture),
    fixtureTypeDMXChannel(typeChannel)
{

}

FixtureDMXChannel::~FixtureDMXChannel()
{
    // Cleanup: Unregister all logical channels
    for (SubFixtureChannel* sfc : logicalChannels) {
        if (sfc != nullptr) {
            sfc->physicalChannel = nullptr;
        }
    }
    logicalChannels.clear();
}

void FixtureDMXChannel::registerLogicalChannel(SubFixtureChannel* sfc)
{
    if (sfc == nullptr) return;

    outputLock.enter();

    if (!logicalChannels.contains(sfc)) {
        logicalChannels.add(sfc);
        sfc->physicalChannel = this;
    }

    // Sort by DMX range max value (y component) descending
    std::sort(logicalChannels.begin(), logicalChannels.end(), [](SubFixtureChannel* a, SubFixtureChannel* b) {
        if (a->parentFixtureTypeChannel == nullptr || b->parentFixtureTypeChannel == nullptr) return false;
        float aRangeMax = a->parentFixtureTypeChannel->dmxRange->getValue()[1];
        float bRangeMax = b->parentFixtureTypeChannel->dmxRange->getValue()[1];
        return aRangeMax > bRangeMax;
    });

    outputLock.exit();
}

void FixtureDMXChannel::unregisterLogicalChannel(SubFixtureChannel* sfc)
{
    if (sfc == nullptr) return;

    outputLock.enter();

    logicalChannels.removeAllInstancesOf(sfc);
    if (sfc->physicalChannel == this) {
        sfc->physicalChannel = nullptr;
    }

    resolveAndOutput();

    outputLock.exit();
}

void FixtureDMXChannel::onLogicalChannelChanged(SubFixtureChannel* sfc)
{
    resolveAndOutput();
}

void FixtureDMXChannel::resolveAndOutput()
{
    outputLock.enter();

    resolveActiveLogicalChannel();
    outputToHardware();

    outputLock.exit();
}

void FixtureDMXChannel::resolveActiveLogicalChannel()
{
    activeLogicalChannel = nullptr;
    
    if (logicalChannels.isEmpty()) {
        return;
    }

    // For now, use highest range resolution (array is kept sorted by DMX range)
    // TODO: Implement other resolution modes, LTP, highlight contention?
    for (SubFixtureChannel* sfc : logicalChannels) {
        if (sfc != nullptr && sfc->isContributing()) {
            activeLogicalChannel = sfc;
            break;
        }
    }
}

void FixtureDMXChannel::outputToHardware()
{
    if (parentFixture != nullptr && fixtureTypeDMXChannel != nullptr) {
        float value = 0.f;

        if (activeLogicalChannel != nullptr) {
            value = activeLogicalChannel->currentValue;

            if (activeLogicalChannel->invertOutput) {
                value = 1 - value;
            }
        } else if (fixtureTypeDMXChannel->defaultValue->enabled) {
            value = fixtureTypeDMXChannel->defaultValue->floatValue();
        }

        int deltaAdress = fixtureTypeDMXChannel->dmxDelta->intValue();
        deltaAdress--;

        for (int i = 0; i < parentFixture->patchs.items.size(); i++) {
            FixturePatch* p = parentFixture->patchs.items[i];
            if (p->enabled->boolValue()) {

                DMXInterface* out = static_cast<DMXInterface*>(p->targetInterface->targetContainer.get());
                if (out != nullptr) {
                    FixturePatch* patch = p;
                    int address = patch->address->getValue();
                    float localValue = value;
                    
                    if (activeLogicalChannel != nullptr) {
                        for (int iCorr = 0; iCorr < patch->corrections.items.size(); iCorr++) {
                            FixturePatchCorrection* c = patch->corrections.items[iCorr];
                            if (c->isOn && c->enabled->boolValue() && (int)c->subFixtureId->getValue() == activeLogicalChannel->subFixtureId && static_cast<ChannelType*>(c->channelType->targetContainer.get()) == activeLogicalChannel->channelType) {
                                if (c->invertChannel->getValue()) {
                                    localValue = 1 - localValue;
                                }
                                localValue = localValue + (float)c->offsetValue->getValue();
                                localValue = jmin((float)1, localValue);
                                localValue = jmax((float)0, localValue);
                                localValue = c->curve.getValueAtPosition(localValue);
                            }
                        }
                    }

                    if (address > 0) {
                        address += (deltaAdress);
                        if (fixtureTypeDMXChannel->resolution->stringValue() == "8bits") {
                            int val = round(jmap(localValue, 0.f, 1.f, activeDMXRangeMin(), activeDMXRangeMax()));
                            //int val = localValue >= 1 ? 255 : round(255 * localValue);
                            val = jlimit(0, 255, val);
                            // NLOG("outputToHardware", "outputting 8bit: " + std::to_string(val));
                            out->sendDMXValue(address, val);
                        }
                        else {
                            // 16bit split channels are not supported, so just use the raw value without mapping
                            int val = localValue >= 1 ? 65535 : round(65535.0 * localValue);
                            //int val = floor(65535.0 * value);
                            //val = value > 65535 ? 65535 : val;
                            int valueA = val / 256;
                            int valueB = val % 256;
                            // NLOG("outputToHardware", "outputting 16bit: " + std::to_string(val) + " as " + std::to_string(valueA) + " and " + std::to_string(valueB));
                            out->sendDMXValue(address, valueA);
                            out->sendDMXValue(address + 1, valueB); // TODO: customized fine channel offset?
                        }
                    }
                }
            }
        }

    }
}

float FixtureDMXChannel::activeDMXRangeMin(){
    if (activeLogicalChannel == nullptr) { return 0.f; }

    return activeLogicalChannel->parentFixtureTypeChannel->dmxRange->x;
}

float FixtureDMXChannel::activeDMXRangeMax(){
    if (activeLogicalChannel == nullptr) {
        if (fixtureTypeDMXChannel->resolution->stringValue() == "8bits") { return 256.f; }
        return 65535.f;
    }

    return activeLogicalChannel->parentFixtureTypeChannel->dmxRange->y;
}
