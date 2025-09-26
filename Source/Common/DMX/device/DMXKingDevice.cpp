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

	// Add port selection parameter
	outputPort = outputCC->addEnumParameter("Output Port", "Select which physical output port to use");
	outputPort->addOption("Port 1", 1);
	outputPort->addOption("Port 2", 2);
	outputPort->addOption("Port 3", 3);
	outputPort->addOption("Port 4", 4);
	outputPort->addOption("Port 5", 5);
	outputPort->addOption("Port 6", 6);
	outputPort->addOption("Port 7", 7);
	outputPort->addOption("Port 8", 8);
	outputPort->setValue(1); // Default to Port 1
}

DMXKingDevice::~DMXKingDevice()
{
	if (DMXKingManager::getInstanceWithoutCreating())
	{
		DMXKingManager::getInstance()->unregisterDevice(this);
	}
}

int DMXKingDevice::getCurrentPortNumber() const
{
	return outputPort->getValueData();
}

void DMXKingDevice::setCurrentPort(SerialDevice * port)
{
	// Unregister from old hardware if switching
	if (DMXKingManager::getInstanceWithoutCreating() && dmxPort && sharedHardwareId.isNotEmpty())
	{
		DMXKingManager::getInstance()->unregisterDevice(this);
	}

	// Call parent implementation
	DMXSerialDevice::setCurrentPort(port);

	// Register with new shared hardware
	if (port)
	{
		sharedHardwareId = port->info->deviceID;
		DMXKingManager::getInstance()->registerDevice(this, port);
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
    int portNumber = getCurrentPortNumber();

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

void DMXKingDevice::onContainerParameterChanged(Parameter * p)
{
	DMXSerialDevice::onContainerParameterChanged(p);

	if (p == portParam)
	{
		// Serial port changed - update hardware registration
		SerialDevice* newPort = portParam->getDevice();
		setCurrentPort(newPort);
	}
	else if (p == outputPort)
	{
		// Output port selection changed - update registration if needed
		updatePortRegistration();
	}
}

void DMXKingDevice::updatePortRegistration()
{
	// If we're connected to hardware, re-register to update port mapping
	if (!sharedHardwareId.isEmpty() && dmxPort)
	{
		DMXKingManager::getInstance()->unregisterDevice(this);
		DMXKingManager::getInstance()->registerDevice(this, dmxPort);
	}
}
