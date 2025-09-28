/*
  ==============================================================================

	DMXKingDevice.cpp
	Created: 25 Sep 2025
	Author:  j-mutter

  ==============================================================================
*/

#include "DMXKingDevice.h"
#include "DMXKingManager.h"

DMXKingDevice::DMXKingDevice() :
	DMXSerialDevice("DMXKing eDMX PRO", DMXKING, true)
{
	inputCC->enabled->setValue(false);

	// Start with just Port A, will be updated when device capabilities are detected
	outputPort = outputCC->addEnumParameter("Output Port", "Select which physical output port to use");
	outputPort->addOption("Port A", 1);
	outputPort->setValueWithKey("Port A");
}

DMXKingDevice::~DMXKingDevice()
{
	if (DMXKingManager::getInstanceWithoutCreating())
	{
		DMXKingManager::getInstance()->unregisterDevice(this);
	}
}

int DMXKingDevice::getCurrentOutputPortNumber() const
{
	return outputPort->getValueData();
}

void DMXKingDevice::setCurrentPort(SerialDevice * port)
{
	if (DMXKingManager::getInstanceWithoutCreating() && dmxPort && sharedHardwareId.isNotEmpty())
	{
		DMXKingManager::getInstance()->unregisterDevice(this);
	}

	DMXSerialDevice::setCurrentPort(port);

	if (port)
	{
		sharedHardwareId = port->info->deviceID;
		DMXKingManager::getInstance()->registerDevice(this, port);

		// Check if port count has already been detected for this hardware
		int detectedOutputPortCount = DMXKingManager::getInstance()->getOutputPortCount(sharedHardwareId);
		if (detectedOutputPortCount > 0)
		{
			onPortCountDetected(detectedOutputPortCount);
		}
	}
	else
	{
		sharedHardwareId = "";
	}
}

void DMXKingDevice::setPortConfig()
{
	// Port configuration is handled by DMXKingManager for shared hardware
	// This device doesn't directly configure the port
}

void DMXKingDevice::sendDMXValuesSerialInternal()
{
    // Send data through the shared manager with the selected port
    int portNumber = getCurrentOutputPortNumber();

	if (!outputCC->enabled->boolValue() || sharedHardwareId.isEmpty() || portNumber < 1 || portNumber > 8)
		return;

	DMXKingManager::getInstance()->sendDMXData(sharedHardwareId, portNumber, dmxDataOut);
}

void DMXKingDevice::onSharedHardwareConnected()
{
	setConnected(true);
}

void DMXKingDevice::onSharedHardwareDisconnected()
{
	setConnected(false);
}

void DMXKingDevice::onPortCountDetected(int portCount)
{
	// Update the output port parameter options based on detected port count
	outputPort->clearOptions();

	for (int i = 1; i <= portCount; ++i)
	{
		char portLetter = 'A' + (i - 1);
		outputPort->addOption("Port " + String::charToString(portLetter), i);
	}

	int currentPort = getCurrentOutputPortNumber();
	if (currentPort > portCount)
	{
		outputPort->setValueWithKey("Port A");
	}
}

void DMXKingDevice::onContainerParameterChanged(Parameter * p)
{
	DMXSerialDevice::onContainerParameterChanged(p);

	if (p == portParam)
	{
		SerialDevice* newPort = portParam->getDevice();
		setCurrentPort(newPort);
	}
	else if (p == outputPort)
	{
		updatePortRegistration();
	}
}

void DMXKingDevice::updatePortRegistration()
{
	if (!sharedHardwareId.isEmpty() && dmxPort)
	{
		DMXKingManager::getInstance()->unregisterDevice(this);
		DMXKingManager::getInstance()->registerDevice(this, dmxPort);
	}
}
