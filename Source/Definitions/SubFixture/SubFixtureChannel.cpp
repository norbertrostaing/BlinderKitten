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
	cuelistStack.clear(false);
	programmerStack.clear(false);
	effectStack.clear(false);
	carouselStack.clear(false);
	cuelistFlashStack.clear(false);
}

SubFixtureChannel::~SubFixtureChannel()
{
	//isDeleted = true;
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
			DMXInterface* out = dynamic_cast<DMXInterface*>(patchs[i]->targetInterface->targetContainer.get());
			if (out != nullptr) {
				int address = patchs[i]->address->getValue();
				if (address > 0) {
					address += (deltaAdress);
					if (chanRes == "8bits") {
						int val = floor(256.0 * v);
						val = val > 255 ? 255 : val;
						out->sendDMXValue(address, val);
					}
					else if(chanRes == "16bits") {
						int val = floor(65535.0 * v);
						val = value > 65535 ? 65535 : val;
						int valueA = val / 256;
						int valueB = val % 256;
						out->sendDMXValue(address, valueA);
						out->sendDMXValue(address+1, valueB);
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
		newValue = cuelistStack[i]->applyToChannel(this, newValue, now);
		ChannelValue* cv = cuelistStack[i]->activeValues.getReference(this);
		if (cv != nullptr && cv->isEnded) {
			overWritten = i-1;
		}
	}

	postCuelistValue = newValue;

	for (int i = 0; i <= overWritten; i++) {
		ChannelValue* cv = cuelistStack[i]->activeValues.getReference(this);
		if (cv != nullptr && !cv->isOverWritten) {
			cv-> isOverWritten = true;
			Brain::getInstance()->pleaseUpdate(cuelistStack[i]);
		}

	}
	for (int i = 0; i < programmerStack.size(); i++) {
		newValue = programmerStack[i]->applyToChannel(this, newValue, now);
	}

	for (int i = 0; i < carouselStack.size(); i++) {
		newValue = carouselStack[i]->applyToChannel(this, newValue, now);
	}

	for (int i = 0; i < effectStack.size(); i++) {
		newValue = effectStack[i]->applyToChannel(this, newValue, now);
	}

	for (int i = 0; i < cuelistFlashStack.size(); i++)
	{
		newValue = cuelistFlashStack[i]->applyToChannel(this, newValue, now, true);
	}


	if (swopKillable) {
		if (Brain::getInstance()->isSwopping) {
			newValue = defaultValue;
			for (int i = 0; i < cuelistFlashStack.size(); i++)
			{
				if (cuelistFlashStack[i]->isSwopping) {
					newValue = cuelistFlashStack[i]->applyToChannel(this, newValue, now, true);
				}
			}
		}
	}

	writeValue(newValue);
}

void SubFixtureChannel::cuelistOnTopOfStack(Cuelist* c) {
	if (cuelistStack.indexOf(c) >= 0) {
		cuelistStack.removeObject(c);
	}
	cuelistStack.add(c);
}

void SubFixtureChannel::cuelistOutOfStack(Cuelist* c) {
	cuelistStack.removeObject(c);
}

void SubFixtureChannel::cuelistOnTopOfFlashStack(Cuelist* c) {
	if (cuelistFlashStack.indexOf(c) >= 0) {
		cuelistFlashStack.removeObject(c);
	}
	cuelistFlashStack.add(c);
}

void SubFixtureChannel::cuelistOutOfFlashStack(Cuelist* c) {
	cuelistFlashStack.removeObject(c);
}

void SubFixtureChannel::programmerOnTopOfStack(Programmer* p) {
	if (programmerStack.indexOf(p) >= 0) {
		programmerStack.removeObject(p);
	}
	programmerStack.add(p);
}

void SubFixtureChannel::programmerOutOfStack(Programmer* p) {
	programmerStack.removeObject(p);
}

void SubFixtureChannel::effectOnTopOfStack(Effect* f) {
	if (effectStack.indexOf(f) >= 0) {
		effectStack.removeObject(f);
	}
	effectStack.add(f);
}

void SubFixtureChannel::effectOutOfStack(Effect* f) {
	effectStack.removeObject(f);
}

void SubFixtureChannel::carouselOnTopOfStack(Carousel* f) {
	if (carouselStack.indexOf(f) >= 0) {
		carouselStack.removeObject(f);
	}
	carouselStack.add(f);
}

void SubFixtureChannel::carouselOutOfStack(Carousel* f) {
	carouselStack.removeObject(f);
}

