/*
  ==============================================================================

    DMXKingDevice.h
    Created: 25 Sep 2025
    Author:  j-mutter

  ==============================================================================
*/

#pragma once

class DMXKingManager;

class DMXKingDevice :
	public DMXSerialDevice
{
public:
	DMXKingDevice();
	~DMXKingDevice();

	// Port selection parameter
	EnumParameter* outputPort;
	int getCurrentPortNumber() const;

	// Override DMXSerialDevice methods to use shared hardware
	void setCurrentPort(SerialDevice * port);
	void setPortConfig() override;
	void sendDMXValuesSerialInternal() override;

	// Called by DMXKingManager when shared hardware connects/disconnects
	void onSharedHardwareConnected();
	void onSharedHardwareDisconnected();

	// Override parameter changes to handle port selection
	void onContainerParameterChanged(Parameter * p) override;

private:
	String sharedHardwareId;
	void updatePortRegistration();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DMXKingDevice)
};
