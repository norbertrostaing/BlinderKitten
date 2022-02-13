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

SubFixtureChannel::SubFixtureChannel()
{
}

SubFixtureChannel::~SubFixtureChannel()
{
}

void SubFixtureChannel::writeValue(float v) {
	v= jmin((float)1, v);
	v= jmax((float)0, v);

	if (parentFixture != nullptr && parentFixtureTypeChannel != nullptr && parentParamDefinition != nullptr) {
		int deltaAdress = parentFixtureTypeChannel->dmxDelta->getValue();
		deltaAdress--;
		String resolution = parentFixtureTypeChannel->resolution->getValue();
		Array<FixturePatch*> patchs = parentFixture->patchs.getItemsWithType<FixturePatch>();
		for (int i = 0; i < patchs.size(); i++) {
			DMXInterface* out = dynamic_cast<DMXInterface*>(patchs[i]->targetInterface->targetContainer.get());
			if (out != nullptr) {
				int address = patchs[i]->address->getValue();
				if (address > 0) {
					address += (deltaAdress);
					if (resolution == "8bits") {
						int value = floor(256.0 * v);
						value = value > 255 ? 255 : value;
						out->sendDMXValue(address, value);
					}
					else if(resolution == "16bits") {
						int value = floor(65535.0 * v);
						value = value > 65535 ? 65535 : value;
						int valueA = value / 256;
						int valueB = value % 256;
						out->sendDMXValue(address, valueA);
						out->sendDMXValue(address+1, valueB);
					}
				}
			}
		}

	}

}


void SubFixtureChannel::updateVal(double now) {
	float value = defaultValue;
	
	int overWritten = -1;
	for (int i = 0; i < cuelistStack.size(); i++)
	{
		value = cuelistStack[i]->applyToChannel(this, value, now);
		ChannelValue* cv = cuelistStack[i]->activeValues.getReference(this);
		if (cv != nullptr && cv->isEnded) {
			overWritten = i-1;
		}
	}

	postCuelistValue = value;

	for (int i = 0; i <= overWritten; i++) {
		ChannelValue* cv = cuelistStack[i]->activeValues.getReference(this);
		if (cv != nullptr && !cv->isOverWritten) {
			cv-> isOverWritten = true;
			Brain::getInstance()->pleaseUpdate(cuelistStack[i]);
		}

	}
	for (int i = 0; i < programmerStack.size(); i++) {
		value = programmerStack[i]->applyToChannel(this, value, now);
	}

	for (int i = 0; i < effectStack.size(); i++) {
		value = effectStack[i]->applyToChannel(this, value, now);
	}

	for (int i = 0; i < cuelistFlashStack.size(); i++)
	{
		value = cuelistFlashStack[i]->applyToChannel(this, value, now, true);
	}


	if (swopKillable) {
		if (Brain::getInstance()->isSwopping) {
			value = defaultValue;
			for (int i = 0; i < cuelistFlashStack.size(); i++)
			{
				if (cuelistFlashStack[i]->isSwopping) {
					value = cuelistFlashStack[i]->applyToChannel(this, value, now, true);
				}
			}
		}
	}

	writeValue(value);
}

void SubFixtureChannel::cuelistOnTopOfStack(Cuelist* c) {
	if (cuelistStack.indexOf(c) >= 0) {
		cuelistStack.removeAllInstancesOf(c);
	}
	cuelistStack.add(c);
}

void SubFixtureChannel::cuelistOutOfStack(Cuelist* c) {
	cuelistStack.removeAllInstancesOf(c);
}

void SubFixtureChannel::cuelistOnTopOfFlashStack(Cuelist* c) {
	if (cuelistFlashStack.indexOf(c) >= 0) {
		cuelistFlashStack.removeAllInstancesOf(c);
	}
	cuelistFlashStack.add(c);
}

void SubFixtureChannel::cuelistOutOfFlashStack(Cuelist* c) {
	cuelistFlashStack.removeAllInstancesOf(c);
}

void SubFixtureChannel::programmerOnTopOfStack(Programmer* p) {
	if (programmerStack.indexOf(p) >= 0) {
		programmerStack.removeAllInstancesOf(p);
	}
	programmerStack.add(p);
}

void SubFixtureChannel::programmerOutOfStack(Programmer* p) {
	programmerStack.removeAllInstancesOf(p);
}

void SubFixtureChannel::effectOnTopOfStack(Effect* f) {
	if (effectStack.indexOf(f) >= 0) {
		effectStack.removeAllInstancesOf(f);
	}
	effectStack.add(f);
}

void SubFixtureChannel::effectOutOfStack(Effect* f) {
	effectStack.removeAllInstancesOf(f);
}

