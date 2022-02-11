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

SubFixtureChannel::SubFixtureChannel(var params) :
	BaseItem(params.getProperty("name", "SubFixtureChannel")),
	objectType(params.getProperty("type", "SubFixtureChannel").toString()),
	objectData(params)
{
	saveAndLoadRecursiveData = true;
	editorIsCollapsed = true;

	userCanDuplicate = false;
	userCanRemove = false;
	nameCanBeChangedByUser = false;

	value = addFloatParameter("Value","Current value of this channel",0,0,1);
	value->setEnabled(false);
	value ->isControllableFeedbackOnly = true;

	itemDataType = "SubFixtureChannel";

	channelType = addTargetParameter("Channel type", "Type of data of this channel", ChannelFamilyManager::getInstance());
	channelType->targetType = TargetParameter::CONTAINER;
	channelType->setEnabled(false);

	resolution = addStringParameter("Resolution", "", "");
	resolution->setEnabled(false);

}

SubFixtureChannel::~SubFixtureChannel()
{
}

void SubFixtureChannel::writeValue(float v) {

	if (value != nullptr) {
		value->setValue(v);
	}

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
						out->send16BitDMXValue(address, value, DMXInterface::MSB);
					}
				}
			}
		}

	}

}


void SubFixtureChannel::updateVal(double now) {
	float value = 0;
	int overWritten = -1;
	for (int i = 0; i < cuelistStack.size(); i++)
	{
		value = cuelistStack[i]->applyToChannel(this, value, now);
		ChannelValue* cv = cuelistStack[i]->activeValues.getReference(this);
		if (cv != nullptr && cv->isEnded) {
			overWritten = i-1;
		}
	}

	for (int i = 0; i <= overWritten; i++) {
		ChannelValue* cv = cuelistStack[i]->activeValues.getReference(this);
		if (cv != nullptr && !cv->isOverWritten) {
			cv-> isOverWritten = true;
			Brain::getInstance()->pleaseUpdate(cuelistStack[i]);
		}

	}

	for (int i = 0; i < effectStack.size(); i++) {
		value = effectStack[i]->applyToChannel(this, value, now);
	}

	for (int i = 0; i < programmerStack.size(); i++) {
		value = programmerStack[i]->applyToChannel(this, value, now);
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

