/*
  ==============================================================================

    DMXKingManager.cpp
    Created: 26 Sep 2025
    Author:  j-mutter

  ==============================================================================
*/

#include "DMXKingManager.h"
#include "DMXKingDevice.h"

juce_ImplementSingleton(DMXKingManager)

DMXKingManager::DMXKingManager()
{
}

DMXKingManager::~DMXKingManager()
{
    HashMap<String, SharedHardware*>::Iterator it(hardwareMap);
    while (it.next())
    {
        SharedHardware* hw = it.getValue();
        if (hw->serialDevice)
        {
            hw->serialDevice->removeSerialDeviceListener(this);
            hw->serialDevice->close();
        }
        delete hw;
    }
    hardwareMap.clear();
}

void DMXKingManager::registerDevice(DMXKingDevice* device, SerialDevice* serialDevice)
{
    if (!serialDevice) return;

    String serialPortId = serialDevice->info->deviceID;
    NLOG("DMXKingManager: ", "registerDevice " + serialPortId);

    SharedHardware* hardware = getOrCreateHardware(serialDevice);
    hardware->connectedDevices.addIfNotAlreadyThere(device);
}

void DMXKingManager::unregisterDevice(DMXKingDevice* device)
{
    HashMap<String, SharedHardware*>::Iterator it(hardwareMap);
    while (it.next())
    {
        SharedHardware* hw = it.getValue();
        hw->connectedDevices.removeAllInstancesOf(device);

        // If no devices left, clean up hardware
        if (hw->connectedDevices.isEmpty())
        {
            if (hw->serialDevice)
            {
                hw->serialDevice->removeSerialDeviceListener(this);
                hw->serialDevice->close();
            }
            String key = it.getKey();
            delete hw;
            hardwareMap.remove(key);
            break;
        }
    }
}

DMXKingManager::SharedHardware* DMXKingManager::getOrCreateHardware(SerialDevice* serialDevice)
{
    String serialPortId = serialDevice->info->deviceID;

    if (!hardwareMap.contains(serialPortId))
    {
        SharedHardware* hardware = new SharedHardware();
        hardware->deviceID = serialPortId;
        hardware->serialDevice = serialDevice;

        // Set up serial device listener
        serialDevice->addSerialDeviceListener(this);

        // Configure port if it's open
        if (serialDevice->isOpen())
        {
            setSerialConfig(hardware);
        }

        NLOG("DMXKingManager: ", "Created new SharedHardware for " + serialPortId);
        hardwareMap.set(serialPortId, hardware);
    }
    else
    {
        // Make sure the hardware has the correct serial device reference
        SharedHardware* hardware = hardwareMap[serialPortId];
        if (hardware->serialDevice != serialDevice)
        {
            // Remove listener from old device if it exists
            if (hardware->serialDevice)
            {
                hardware->serialDevice->removeSerialDeviceListener(this);
            }

            // Update to new device
            hardware->serialDevice = serialDevice;
            serialDevice->addSerialDeviceListener(this);

            // Configure port if it's open
            if (serialDevice->isOpen())
            {
                setSerialConfig(hardware);
            }
        }
        NLOG("DMXKingManager: ", "Updated SharedHardware for " + serialPortId);
    }

    return hardwareMap[serialPortId];
}

void DMXKingManager::sendDMXData(const String& serialPortId, int outputPort, const uint8* data)
{
    SharedHardware* hardware = hardwareMap[serialPortId];
    if (!hardware || !hardware->serialDevice || !hardware->serialDevice->isOpen())
        return;

    try
    {
        // Determine port label based on port number (1-8)
        uint8 outputPortLabel = DMXKING_SEND_PORT1_LABEL + (outputPort - 1);

        uint8 headerData[5] = {
            DMXKING_START_MESSAGE,
            outputPortLabel,
            (DMXKING_CHANNEL_COUNT + 1) & 255,
            ((DMXKING_CHANNEL_COUNT + 1) >> 8) & 255,
            DMXKING_START_CODE
        };

        uint8 footerData[1] = { DMXKING_END_MESSAGE };

        NLOG("DMXKingManager: ", "Sending DMX data to port " + String(outputPort) + " (label 0x" + String::toHexString(outputPortLabel) + ")");

        hardware->serialDevice->port->write(headerData, 5);
        hardware->serialDevice->port->write(data, 512);
        hardware->serialDevice->port->write(footerData, 1);
        hardware->serialDevice->port->flush();
    }
    catch (serial::IOException e)
    {
        DBG("IO Exception on DMXKing port " << serialPortId << ": " << e.what());
    }
    catch (serial::PortNotOpenedException)
    {
        DBG("Port Not Opened on DMXKing port " << serialPortId);
    }
    catch (serial::SerialException e)
    {
        DBG("Serial Exception on DMXKing port " << serialPortId << ": " << e.what());
    }
}

void DMXKingManager::setSerialConfig(SharedHardware* hardware)
{
    if (!hardware->serialDevice) return;

    hardware->serialDevice->port->setBaudrate(115200);
    hardware->serialDevice->port->setBytesize(serial::eightbits);
    hardware->serialDevice->port->setStopbits(serial::stopbits_one);
    hardware->serialDevice->port->setParity(serial::parity_none);
    hardware->serialDevice->port->flush();

    // Get serial number
    Array<uint8> getSerialNumberBytes((uint8)0x7E, (uint8)10, (uint8)0, (uint8)0, (uint8)0xE7);
    hardware->serialDevice->writeBytes(getSerialNumberBytes);

    // Enable change-on-data mode
    uint8 changeAlwaysData[6] = { DMXKING_START_MESSAGE, DMXKING_RECEIVE_ON_CHANGE_LABEL, 1, 0, DMXKING_CHANGE_ALWAYS_CODE, DMXKING_END_MESSAGE };
    hardware->serialDevice->port->write(changeAlwaysData, 6);
}

void DMXKingManager::serialDataReceived(const var& data)
{
    // Find which hardware this data belongs to
    SerialDevice* sender = dynamic_cast<SerialDevice*>(data.getObject());
    if (!sender) return;

    SharedHardware* hardware = nullptr;
    HashMap<String, SharedHardware*>::Iterator it(hardwareMap);
    while (it.next())
    {
        SharedHardware* hw = it.getValue();
        if (hw->serialDevice == sender)
        {
            hardware = hw;
            break;
        }
    }

    if (!hardware) return;

    hardware->serialBuffer.addArray((const uint8_t*)data.getBinaryData()->getData(), (int)data.getBinaryData()->getSize());

    int endIndex = 0;
    Array<uint8> packet = getDMXPacket(hardware->serialBuffer, endIndex);
    while (packet.size() > 0)
    {
        processDMXPacket(hardware, packet);
        hardware->serialBuffer.removeRange(0, endIndex);
        packet = getDMXPacket(hardware->serialBuffer, endIndex);
    }
}

void DMXKingManager::portOpened(SerialDevice* port)
{
    HashMap<String, SharedHardware*>::Iterator it(hardwareMap);
    while (it.next())
    {
        SharedHardware* hw = it.getValue();
        if (hw->serialDevice == port)
        {
            setSerialConfig(hw);

            // Notify all connected devices
            for (auto device : hw->connectedDevices)
            {
                device->onSharedHardwareConnected();
            }
            break;
        }
    }
}

void DMXKingManager::portClosed(SerialDevice* port)
{
    HashMap<String, SharedHardware*>::Iterator it(hardwareMap);
    while (it.next())
    {
        SharedHardware* hw = it.getValue();
        if (hw->serialDevice == port)
        {
            // Notify all connected devices
            for (auto device : hw->connectedDevices)
            {
                device->onSharedHardwareDisconnected();
            }
            break;
        }
    }
}

void DMXKingManager::portRemoved(SerialDevice* port)
{
    portClosed(port);
}

Array<uint8> DMXKingManager::getDMXPacket(Array<uint8> bytes, int& endIndex)
{
    if (bytes.size() < DMXKING_HEADER_LENGTH + 1) return Array<uint8>();
    int numBytes = bytes.size();

    for (int i = 0; i < numBytes; ++i)
    {
        if (bytes[i] == DMXKING_START_MESSAGE)
        {
            int length = (int)bytes[i + 2] + ((int)bytes[i + 3] << 8);
            if (bytes.size() - i < DMXKING_HEADER_LENGTH + length) continue;
            endIndex = i + DMXKING_HEADER_LENGTH + length;

            if (bytes[endIndex] == DMXKING_END_MESSAGE)
            {
                return Array<uint8>(bytes.getRawDataPointer() + i, DMXKING_HEADER_LENGTH + length);
            }
        }
    }

    return Array<uint8>();
}

void DMXKingManager::processDMXPacket(SharedHardware* hardware, Array<uint8> bytes)
{
    int messageLabel = (int)bytes[1];
    int length = (int)bytes[2] + ((int)bytes[3] << 8);

    switch (messageLabel)
    {
    case DMXKING_RECEIVE_LABEL:
    {
        // Forward to all input-enabled devices
        if (length > DMXKING_CHANNEL_COUNT + 2)
        {
            LOGWARNING("DMXKing bad DMX Length : " << length);
            return;
        }

        if (length > bytes.size())
        {
            LOGWARNING("DMXKing bad data size : " << bytes.size() << " <> " << length);
            return;
        }

        if (bytes[DMXKING_HEADER_LENGTH] != DMXKING_START_CODE)
        {
            LOGWARNING("DMXKing bad first byte " << bytes[0] << " should be " << DMXKING_START_CODE);
            return;
        }

        for (auto device : hardware->connectedDevices)
        {
            if (device->inputCC && device->inputCC->enabled->boolValue())
            {
                device->setDMXValuesIn(length - 1, bytes.getRawDataPointer() + DMXKING_HEADER_LENGTH + 1);
            }
        }
    }
    break;

    case DMXKING_RECEIVE_SERIAL_NUMBER_LABEL:
    {
        int serialNumber = (int)(bytes[4] + (bytes[5] << 8) + (bytes[6] << 16) + (bytes[7] << 24));
        LOG("DMXKing serial number (" << length << ") : " << String::toHexString(serialNumber));
    }
    break;

    default:
        // Handle other messages if needed
        break;
    }
}
