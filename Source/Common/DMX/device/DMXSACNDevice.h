#pragma once

#include "DMXDevice.h"

#pragma warning(push) 
#pragma warning(disable:4201) 
#include "sacn/e131.h"
#pragma warning(pop)


class DMXSACNDevice : public DMXDevice
{
public:
    DMXSACNDevice();
    ~DMXSACNDevice();

    bool isReady = false;
    juce::String getName() const;
    juce::String getDescription() const;

protected:
    IntParameter* universeParam;
    BoolParameter* multicast;
    StringParameter* remoteHost;
    IntParameter* remotePort;
    StringParameter* nodeName;
    IntParameter* priority;
    uint8 dmxBuffer[512];
    e131_packet_t packet;

    void sendDMXValue(int channel, int value) override;
    void sendDMXRange(int startChannel, Array<int> values) override;
    void sendDMXValuesInternal();
    void onContainerParameterChanged(Parameter* p) override;
    void paramPacket();

private:
    juce::DatagramSocket socket;
    juce::Uuid cid;
    uint8_t sequenceNumber = 0;


};
