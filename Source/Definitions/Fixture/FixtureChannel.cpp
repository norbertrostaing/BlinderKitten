/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "FixtureChannel.h"
#include "../FixtureParamType/FixtureParamTypeManager.h"
#include "../DeviceType/DeviceTypeChannel.h"
#include "../FixtureParamType/FixtureParamDefinition/FixtureParamDefinition.h"
#include "Brain.h"
#include "../Device/DevicePatch.h"
#include "../Interface/InterfaceIncludes.h"
#include "../Cuelist/Cuelist.h"
#include "../Programmer/Programmer.h"
#include "../ChannelValue.h"

FixtureChannel::FixtureChannel(var params) :
	BaseItem(params.getProperty("name", "FixtureChannel")),
	objectType(params.getProperty("type", "FixtureChannel").toString()),
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

	itemDataType = "FixtureChannel";

	channelType = addTargetParameter("Channel type", "Type of data of this channel", FixtureParamTypeManager::getInstance());
	channelType->targetType = TargetParameter::CONTAINER;
	channelType->setEnabled(false);

	resolution = addStringParameter("Resolution", "", "");
	resolution->setEnabled(false);

}

FixtureChannel::~FixtureChannel()
{
}

void FixtureChannel::writeValue(float v) {

	if (value != nullptr) {
		value->setValue(v);
	}

	if (parentDevice != nullptr && parentDeviceTypeChannel != nullptr && parentParamDefinition != nullptr) {
		int deltaAdress = parentDeviceTypeChannel->dmxDelta->getValue();
		deltaAdress--;
		String resolution = parentDeviceTypeChannel->resolution->getValue();
		Array<DevicePatch*> patchs = parentDevice->patchs->getItemsWithType<DevicePatch>();
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


void FixtureChannel::updateVal(int64 now) {
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

	for (int i = 0; i < programmerStack.size(); i++) {
		value = programmerStack[i]->applyToChannel(this, value, now);
	}

	writeValue(value);
}

void FixtureChannel::cuelistOnTopOfStack(Cuelist* c) {
	if (cuelistStack.indexOf(c) >= 0) {
		cuelistStack.removeAllInstancesOf(c);
	}
	cuelistStack.add(c);
}

void FixtureChannel::cuelistOutOfStack(Cuelist* c) {
	cuelistStack.removeAllInstancesOf(c);
}

void FixtureChannel::programmerOnTopOfStack(Programmer* p) {
	if (programmerStack.indexOf(p) >= 0) {
		programmerStack.removeAllInstancesOf(p);
	}
	programmerStack.add(p);
}

void FixtureChannel::programmerOutOfStack(Programmer* p) {
	programmerStack.removeAllInstancesOf(p);
}

