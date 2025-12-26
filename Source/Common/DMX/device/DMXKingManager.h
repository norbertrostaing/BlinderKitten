/*
  ==============================================================================

    DMXKingManager.h
    Created: 26 Sep 2025
    Author:  j-mutter

  ==============================================================================
*/

#pragma once

#define DMXKING_START_MESSAGE 0x7E
#define DMXKING_END_MESSAGE 0xE7

#define DMXKING_SEND_PORT1_LABEL 0x64
#define DMXKING_SEND_PORT2_LABEL 0x65
#define DMXKING_SEND_PORT3_LABEL 0x66
#define DMXKING_SEND_PORT4_LABEL 0x67
#define DMXKING_SEND_PORT5_LABEL 0x68
#define DMXKING_SEND_PORT6_LABEL 0x69
#define DMXKING_SEND_PORT7_LABEL 0x6A
#define DMXKING_SEND_PORT8_LABEL 0x6B

#define DMXKING_RECEIVE_LABEL 5
#define DMXKING_RECEIVE_ON_CHANGE_LABEL 8
#define DMXKING_RECEIVE_SERIAL_NUMBER_LABEL 10

#define DMXKING_DMX_PORT_COUNT_LABEL 0x63
#define DMXKING_DMX_PORT_DIRECTION_LABEL 0x71

#define DMXKING_START_CODE 0
#define DMXKING_CHANNEL_COUNT 512

#define DMXKING_CHANGE_ALWAYS_CODE 0
#define DMXKING_HEADER_LENGTH 4

class DMXKingDevice;

class DMXKingManager :
    public SerialDevice::SerialDeviceListener
{
public:
    juce_DeclareSingleton(DMXKingManager, true)
    DMXKingManager();
    ~DMXKingManager();

    struct SharedHardware
    {
        SerialDevice* serialDevice;
        String deviceID;
        Array<DMXKingDevice*> connectedDevices;
        Array<uint8> serialBuffer;

        int detectedOutputPortCount;
        bool outputPortCountDetected;

        SharedHardware() : serialDevice(nullptr), detectedOutputPortCount(1), outputPortCountDetected(false) {}
    };

    HashMap<String, SharedHardware*> hardwareMap;

    // Register/unregister devices for a specific serial port
    void registerDevice(DMXKingDevice* device, SerialDevice* serialDevice);
    void unregisterDevice(DMXKingDevice* device);

    // Shared serial communication
    SharedHardware* getOrCreateHardware(SerialDevice* serialDevice);
    void sendDMXData(const String& serialPortId, int outputPort, const uint8* data);
    void setSerialConfig(SharedHardware* hardware);
    void pollDeviceCapabilities(SharedHardware* hardware);

    // Ouput Port count detection
    int getOutputPortCount(const String& serialPortId);
    void notifyDevicesOutputPortCountChanged(SharedHardware* hardware);

    // SerialDeviceListener implementation
    void serialDataReceived(const var &data) override;
    void portOpened(SerialDevice *) override;
    void portClosed(SerialDevice *) override;
    void portRemoved(SerialDevice *) override;

    void flushInputCustom(SharedHardware* hardware);

private:
    void processDMXPacket(SharedHardware* hardware, Array<uint8> bytes);
    Array<uint8> getDMXPacket(Array<uint8> bytes, int &endIndex);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DMXKingManager)
};
