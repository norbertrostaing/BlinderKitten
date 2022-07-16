/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "SubFixtureChannel.h"
#include "../ChannelFamily/ChannelFamilyManager.h"
#include "../FixtureType/FixtureTypeChannel.h"
#include "../ChannelFamily/ChannelType/ChannelType.h"
#include "Brain.h"
#include "../Fixture/FixturePatch.h"
#include "../Interface/InterfaceIncludes.h"
#include "../Cuelist/Cuelist.h"
#include "../Programmer/Programmer.h"
#include "../Effect/Effect.h"
#include "../ChannelValue.h"
#include "UI/InputPanel.h"

SubFixtureChannel::SubFixtureChannel()
{
	cuelistStack.clear();
	programmerStack.clear();
	effectStack.clear();
	carouselStack.clear();
	cuelistFlashStack.clear();
}

SubFixtureChannel::~SubFixtureChannel()
{
	//isDeleted = true;
	Brain::getInstance()->grandMasterChannels.removeAllInstancesOf(this);
}

void SubFixtureChannel::writeValue(float v) {
	v= jmin((float)1, v);
	v= jmax((float)0, v);


	if (parentFixture != nullptr && parentFixtureTypeChannel != nullptr && parentParamDefinition != nullptr) {

		int deltaAdress = parentFixtureTypeChannel->dmxDelta->getValue();
		deltaAdress--;
		String chanRes = parentFixtureTypeChannel->resolution->getValue();
		Array<FixturePatch*> patchs = parentFixture->patchs.getItemsWithType<FixturePatch>();
		for (int i = 0; i < patchs.size(); i++) {
			value = v;

			DMXInterface* out = dynamic_cast<DMXInterface*>(patchs.getReference(i)->targetInterface->targetContainer.get());
			if (out != nullptr) {
				FixturePatch* patch = patchs.getReference(i);
				int address = patch->address->getValue();

				for (int i = 0; i < patch->corrections.items.size(); i++) {
					FixturePatchCorrection* c = patch->corrections.items[i];
					if (c->isOn && (int)c->subFixtureId->getValue() == subFixtureId && dynamic_cast<ChannelType*>(c->channelType->targetContainer.get()) == channelType) {
						if (c->invertChannel->getValue()) {
							value = 1 - value;
						}
						value = value + (float)c->offsetValue->getValue();
						value = jmin((float)1, value);
						value = jmax((float)0, value);
						value = c->curve.getValueAtPosition(value);
					}
				}


				if (address > 0) {
					address += (deltaAdress);
					if (chanRes == "8bits") {
						int val = floor(256.0 * value);
						val = val > 255 ? 255 : val;
						out->sendDMXValue(address, val);
					}
					else if (chanRes == "16bits") {
						int val = floor(65535.0 * value);
						val = value > 65535 ? 65535 : val;
						int valueA = val / 256;
						int valueB = val % 256;
						out->sendDMXValue(address, valueA);
						out->sendDMXValue(address + 1, valueB);
					}
				}
			}
		}

	}

}


void SubFixtureChannel::updateVal(double now) {
	float newValue = defaultValue;

	int overWritten = -1;
	for (int i = 0; i < cuelistStack.size(); i++)
	{
		newValue = cuelistStack.getReference(i)->applyToChannel(this, newValue, now);
		ChannelValue* cv = cuelistStack.getReference(i)->activeValues.getReference(this);
		if (cv != nullptr && cv->isEnded) {
			overWritten = i - 1;
		}
	}

	postCuelistValue = newValue;

	for (int i = 0; i <= overWritten; i++) {
		ChannelValue* cv = cuelistStack.getReference(i)->activeValues.getReference(this);
		if (cv != nullptr && !cv->isOverWritten) {
			cv->isOverWritten = true;
			Brain::getInstance()->pleaseUpdate(cuelistStack.getReference(i));
		}
	}
	for (int i = 0; i < programmerStack.size(); i++) {
		newValue = programmerStack.getReference(i)->applyToChannel(this, newValue, now);
	}

	for (int i = 0; i < mapperStack.size(); i++) {
		newValue = mapperStack.getReference(i)->applyToChannel(this, newValue, now);
	}

	for (int i = 0; i < carouselStack.size(); i++) {
		newValue = carouselStack.getReference(i)->applyToChannel(this, newValue, now);
	}

	for (int i = 0; i < effectStack.size(); i++) {
		newValue = effectStack.getReference(i)->applyToChannel(this, newValue, now);
	}

	for (int i = 0; i < cuelistFlashStack.size(); i++)
	{
		newValue = cuelistFlashStack.getReference(i)->applyToChannel(this, newValue, now, true);
	}


	if (swopKillable) {
		if (Brain::getInstance()->isSwopping) {
			newValue = defaultValue;
			for (int i = 0; i < cuelistFlashStack.size(); i++)
			{
				if (cuelistFlashStack.getReference(i)->isSwopping) {
					newValue = cuelistFlashStack.getReference(i)->applyToChannel(this, newValue, now, true);
				}
			}
		}
	}

	if (reactToGrandMaster) {
		double gm = InputPanel::getInstance()->grandMaster.getValue();
		newValue *= gm;
	}

	writeValue(newValue);
}

void SubFixtureChannel::cuelistOnTopOfStack(Cuelist* c) {
	while (cuelistStack.indexOf(c) >= 0) {
		cuelistStack.removeAllInstancesOf(c);
	}
	cuelistStack.add(c);
}

void SubFixtureChannel::cuelistOutOfStack(Cuelist* c) {
	while (cuelistStack.indexOf(c) >= 0) {
		cuelistStack.removeAllInstancesOf(c);
	}
}

void SubFixtureChannel::cuelistOnTopOfFlashStack(Cuelist* c) {
	while (cuelistFlashStack.indexOf(c) >= 0) {
		cuelistFlashStack.removeAllInstancesOf(c);
	}
	cuelistFlashStack.add(c);
}

void SubFixtureChannel::cuelistOutOfFlashStack(Cuelist* c) {
	while (cuelistFlashStack.indexOf(c) >= 0) {
		cuelistFlashStack.removeAllInstancesOf(c);
	}
}

void SubFixtureChannel::programmerOnTopOfStack(Programmer* p) {
	while (programmerStack.indexOf(p) >= 0) {
		programmerStack.removeAllInstancesOf(p);
	}
	programmerStack.add(p);
}

void SubFixtureChannel::programmerOutOfStack(Programmer* p) {
	while (programmerStack.indexOf(p) >= 0) {
		programmerStack.removeAllInstancesOf(p);
	}
}

void SubFixtureChannel::effectOnTopOfStack(Effect* f) {
	while (effectStack.indexOf(f) >= 0) {
		effectStack.removeAllInstancesOf(f);
	}
	effectStack.add(f);
}

void SubFixtureChannel::effectOutOfStack(Effect* f) {
	while (effectStack.indexOf(f) >= 0) {
		effectStack.removeAllInstancesOf(f);
	}
}

void SubFixtureChannel::carouselOnTopOfStack(Carousel* f) {
	while (carouselStack.indexOf(f) >= 0) {
		carouselStack.removeAllInstancesOf(f);
	}
	carouselStack.add(f);
}

void SubFixtureChannel::carouselOutOfStack(Carousel* f) {
	while (carouselStack.indexOf(f) >= 0) {
		carouselStack.removeAllInstancesOf(f);
	}
}

void SubFixtureChannel::mapperOnTopOfStack(Mapper* f) {
	while (mapperStack.indexOf(f) >= 0) {
		mapperStack.removeAllInstancesOf(f);
	}
	mapperStack.add(f);
}

void SubFixtureChannel::mapperOutOfStack(Mapper* f) {
	while (mapperStack.indexOf(f) >= 0) {
		mapperStack.removeAllInstancesOf(f);
	}
}

