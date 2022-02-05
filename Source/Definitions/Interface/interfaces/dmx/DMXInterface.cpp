/*
  ==============================================================================

	DMXInterface.cpp
	Created: 26 Sep 2020 1:51:42pm
	Author:  bkupe

  ==============================================================================
*/

DMXInterface::DMXInterface() :
	Interface(getTypeString())
{
	dmxType = addEnumParameter("DMX Type", "Choose the type of dmx interface you want to connect");

	dmxType->addOption("Open DMX", DMXDevice::OPENDMX)->addOption("Enttec DMX Pro", DMXDevice::ENTTEC_DMXPRO)->addOption("Enttec DMX MkII", DMXDevice::ENTTEC_MK2)->addOption("Art-Net", DMXDevice::ARTNET);
	dmxType->setValueWithKey("Open DMX");

	dmxConnected = addBoolParameter("Connected", "DMX is connected ?", false);
	dmxConnected->isControllableFeedbackOnly = true;
	dmxConnected->isSavable = false;

	channelTestingMode = addBoolParameter("Channel Testing Mode", "Is testing with the Channel view ?", false);
	channelTestingMode->hideInEditor = true;

	channelTestingFlashValue = addFloatParameter("Channel Testing Flash Value", "Flash value of channel testing", 1, 0, 1);
	channelTestingFlashValue->hideInEditor = true;

	setCurrentDMXDevice(DMXDevice::create((DMXDevice::Type)(int)dmxType->getValueData()));

}

DMXInterface::~DMXInterface()
{
}

void DMXInterface::clearItem()
{
	BaseItem::clearItem();
	setCurrentDMXDevice(nullptr);
}

void DMXInterface::onContainerParameterChanged(Parameter* p)
{
	Interface::onContainerParameterChanged(p);
	if (p == enabled)
	{
		if (dmxDevice != nullptr) dmxDevice->enabled = enabled->boolValue();

	}
	else if (p == dmxType)
	{
		setCurrentDMXDevice(DMXDevice::create((DMXDevice::Type)(int)dmxType->getValueData()));
	}
}

void DMXInterface::setCurrentDMXDevice(DMXDevice* d)
{
	if (dmxDevice.get() == d) return;

	if (dmxDevice != nullptr)
	{
		dmxDevice->removeDMXDeviceListener(this);
		dmxDevice->clearDevice();
		removeChildControllableContainer(dmxDevice.get());
	}

	dmxDevice.reset(d);

	dmxConnected->hideInEditor = dmxDevice == nullptr || dmxDevice->type == DMXDevice::ARTNET;
	dmxConnected->setValue(false);

	if (dmxDevice != nullptr)
	{
		dmxDevice->enabled = enabled->boolValue();
		dmxDevice->addDMXDeviceListener(this);
		addChildControllableContainer(dmxDevice.get());
		dmxConnected->setValue(true);
	}
}

void DMXInterface::sendDMXValue(int channel, int value)
{
	if (!enabled->boolValue() || dmxDevice == nullptr) return;
	if (logOutgoingData->boolValue()) NLOG(niceName, "Send DMX : " + String(channel) + " > " + String(value));
	dmxDevice->sendDMXValue(channel, value);
}

void DMXInterface::sendDMXValues(int startChannel, Array<int> values)
{
	if (!enabled->boolValue() || dmxDevice == nullptr) return;
	if (logOutgoingData->boolValue())
	{
		String s = "Send DMX : " + String(startChannel) + ", " + String(values.size()) + " values";
		int ch = startChannel;
		for (auto& v : values)
		{
			s += "\nChannel " + String(ch) + " : " + String(v);
			ch++;
		}
		NLOG(niceName, s);
	}

	dmxDevice->sendDMXRange(startChannel, values);
}

void DMXInterface::send16BitDMXValue(int startChannel, int value, DMXByteOrder byteOrder)
{
	if (!enabled->boolValue() || dmxDevice == nullptr) return;
	if (logOutgoingData->boolValue()) NLOG(niceName, "Send 16-bit DMX : " + String(startChannel) + " > " + String(value));
	dmxDevice->sendDMXValue(startChannel, byteOrder == MSB ? (value >> 8) & 0xFF : value & 0xFF);
	dmxDevice->sendDMXValue(startChannel + 1, byteOrder == MSB ? 0xFF : (value >> 8) & 0xFF);

}

void DMXInterface::send16BitDMXValues(int startChannel, Array<int> values, DMXByteOrder byteOrder)
{
	if (!enabled->boolValue() || dmxDevice == nullptr) return;
	if (logOutgoingData->boolValue()) NLOG(niceName, "Send 16-bit DMX : " + String(startChannel) + " > " + String(values.size()) + " values");

	Array<int> dmxValues;
	int numValues = values.size();
	dmxValues.resize(numValues * 2);
	for (int i = 0; i < numValues; ++i)
	{
		int value = values[i];
		dmxValues.set(i * 2, byteOrder == MSB ? (value >> 8) & 0xFF : value & 0xFF);
		dmxValues.set(i * 2 + 1, byteOrder == MSB ? 0xFF : (value >> 8) & 0xFF);
	}

	dmxDevice->sendDMXRange(startChannel, dmxValues);
}

void DMXInterface::dmxDeviceConnected()
{
	dmxConnected->setValue(true);
}

void DMXInterface::dmxDeviceDisconnected()
{
	dmxConnected->setValue(false);
}

void DMXInterface::dmxDataInChanged(int numChannels, uint8* values)
{
	if (isClearing || !enabled->boolValue()) return;
	if (logIncomingData->boolValue()) NLOG(niceName, "DMX In : " + String(numChannels) + " channels received.");
}


//void DMXInterface::sendValuesForObject(Object* o)
//{
//	if (channelTestingMode->boolValue()) return;
//
//	Interface::sendValuesForObject(o);
//
//	DMXParams* dmxParams = dynamic_cast<DMXParams*>(o->interfaceParameters.get());
//	
//	jassert(dmxParams != nullptr);
//
//	int startChannel = dmxParams->startChannel->intValue();
//	HashMap<int, float> compValues;
//	
//	for (auto& c : o->componentManager->items)
//	{
//		if (!c->enabled->boolValue()) continue;
//		c->fillOutValueMap(compValues, startChannel);
//	}
//
//	HashMap<int, float>::Iterator it(compValues);
//	while (it.next()) sendDMXValue(it.getKey(), it.getValue() * 255);
//
//}
//


InterfaceUI* DMXInterface::createUI()
{
	return new DMXInterfaceUI(this);
}


// DMX PARAMS
DMXInterface::DMXParams::DMXParams() :
	ControllableContainer("DMX Params")
{

	startChannel = addIntParameter("Start Channel", "The first channel to set the values. The values will automatically distributed depending on the object.", 1, 1, 512);
}

