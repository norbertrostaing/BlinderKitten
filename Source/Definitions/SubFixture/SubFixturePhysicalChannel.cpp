/*
  ==============================================================================

    SubFixturePhysicalChannel.cpp
    Created: 24 Oct 2026 11:23:26pm
    Author:  Norb

  ==============================================================================
*/

#include "SubFixturePhysicalChannel.h"
#include "Brain.h"
#include "SubFixtureChannel.h"
#include "Definitions/FixtureType/FixtureType.h"
#include "Definitions/Fixture/Fixture.h"
#include "Definitions/Interface/InterfaceIncludes.h"

SubFixturePhysicalChannel::SubFixturePhysicalChannel()
{
}

SubFixturePhysicalChannel::~SubFixturePhysicalChannel()
{
}

void SubFixturePhysicalChannel::setDirty()
{
    Brain::getInstance()->setPhysicaChannelDirty(this);
}

void SubFixturePhysicalChannel::writeValue()
{
    SubFixtureChannel* winner = nullptr;
	usingPool.enter();
    for (SubFixtureChannel* sfc : writingPool) {
        if (sfc->isActive) {
            if (winner == nullptr || sfc->logicalPriority >= winner->logicalPriority) {
                winner = sfc;
            }
        }
    }
	usingPool.exit();

    float v = winner != nullptr && winner->currentValue >=0 ? winner->currentValue : 0;
	if (winner == nullptr) return;
	
	if (winner->parentFixture != nullptr && winner->parentFixtureTypeChannel != nullptr && winner->channelType != nullptr) {

		if (winner->parentFixtureTypeChannel->curve.enabled->boolValue()) {
			v = winner->parentFixtureTypeChannel->curve.getValueAtPosition(v);
		}

		int deltaAdress = winner->parentFixtureTypeChannel->dmxDelta->intValue();
		deltaAdress--;
		String chanRes = winner->parentFixtureTypeChannel->resolution->stringValue();
		for (int i = 0; i < winner->parentFixture->patchs.items.size(); i++) {
			FixturePatch* p = winner->parentFixture->patchs.items[i];
			if (p->enabled->boolValue()) {
				float localValue = v;

				DMXInterface* out = static_cast<DMXInterface*>(p->targetInterface->targetContainer.get());
				if (out != nullptr) {
					FixturePatch* patch = p;
					int address = patch->address->getValue();

					for (int iCorr = 0; iCorr < patch->corrections.items.size(); iCorr++) {
						FixturePatchCorrection* c = patch->corrections.items[iCorr];
						if (c->isOn && c->enabled->boolValue() && (int)c->subFixtureId->getValue() == winner->subFixtureId && static_cast<ChannelType*>(c->channelType->targetContainer.get()) == winner->channelType) {
							if (c->invertChannel->getValue()) {
								localValue = 1 - localValue;
							}
							localValue = localValue + (float)c->offsetValue->getValue();
							localValue = jmin((float)1, localValue);
							localValue = jmax((float)0, localValue);
							localValue = c->curve.getValueAtPosition(localValue);
						}
					}

					if (address > 0) {
						localValue = jmap((double)localValue, winner->lowPhysicalValue, winner->highPhysicalValue);
						address += (deltaAdress);
						if (fineChannelDelta == 0) {
							
							int val = localValue >= 1 ? 255 : round(255.0 * localValue);
							out->sendDMXValue(address, val);
						}
						else {
							int val = localValue >= 1 ? 65535 : round(65535.0 * localValue);
							int valueA = val / 256;
							int valueB = val % 256;
							out->sendDMXValue(address, valueA);
							out->sendDMXValue(address + fineChannelDelta, valueB);
						}
					}
				}
			}
		}

	}


}

void SubFixturePhysicalChannel::addSubFixtureChannelToStack(SubFixtureChannel* sfc)
{
	usingPool.enter();
    writingPool.removeAllInstancesOf(sfc);
    writingPool.add(sfc);
	usingPool.exit();
}
