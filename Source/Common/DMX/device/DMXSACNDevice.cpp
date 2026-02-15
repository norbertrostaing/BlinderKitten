#include "DMXSACNDevice.h"
#include "sacn/e131.c"



DMXSACNDevice::DMXSACNDevice()
    : DMXDevice("sACN", SACN, false)
{
    universeParam = addIntParameter("Universe", "universe", 1, 0, 63999);
    nodeName = addStringParameter("Node Name", "Name to advertise", "BlinderKitten");
    multicast = addBoolParameter("Multicast", "check to send this as multicast", true);
    remoteHost = addStringParameter("Remote Host", "IP to which send the sACN to", "127.0.0.1");
    remotePort = addIntParameter("Remote Port", "Port to send data", 5568, 0, 65535);
    priority = addIntParameter("Priority", "Priority of the packets to send", 100, 0, 200);

    for (int i = 0; i< 512; i++) dmxBuffer[i] = 0;
    e131_pkt_init(&packet, 1, 512);
    
    cid = juce::Uuid();
    socket.setEnablePortReuse(true);
    socket.setMulticastLoopbackEnabled(false);
    socket.bindToPort(0);
    int ttl = 1;
    setsockopt(socket.getRawSocketHandle(), IPPROTO_IP, IP_MULTICAST_TTL, (const char*)&ttl, sizeof(ttl));
    isReady = true;
}

DMXSACNDevice::~DMXSACNDevice()
{
    socket.shutdown();
}

juce::String DMXSACNDevice::getName() const
{
    return "sACN";
}

juce::String DMXSACNDevice::getDescription() const
{
    return "Streaming ACN (E1.31) DMX Output";
}

void DMXSACNDevice::sendDMXValue(int channel, int value)
{
    dmxBuffer[channel - 1] = (uint8)value;
    DMXDevice::sendDMXValue(channel, value);
}

void DMXSACNDevice::sendDMXRange(int startChannel, Array<int> values)
{
    int numValues = values.size();
    for (int i = 0; i < numValues; ++i)
    {
        int channel = startChannel + i;
        if (channel < 0) continue;
        if (channel > 512) break;

        dmxBuffer[channel - 1] = (uint8)(values[i]);
    }


    DMXDevice::sendDMXRange(startChannel, values);
}

void DMXSACNDevice::sendDMXValuesInternal()
{
    if (!isReady) return;
    const int universe = universeParam->intValue();

    juce::IPAddress targetIP("127.0.0.1");
    if (multicast->boolValue()) {
        targetIP = juce::IPAddress(239, 255, (universe >> 8) & 0xFF, universe & 0xFF);
    }
    else {
        targetIP = juce::IPAddress(remoteHost->stringValue());
    }

    //LOG("need to send to "+targetIP.toString());

    e131_packet_t* senderPacket = &packet;

    //String ip = remoteHost->stringValue();

    memcpy(senderPacket->dmp.prop_val + 1, dmxBuffer, 512);
    e131_error_t e = e131_pkt_validate(senderPacket);
    if (e != E131_ERR_NONE) LOGWARNING("Packet validation error : " << e131_strerror(e));

    int sent = socket.write(targetIP.toString(), remotePort->intValue(), senderPacket, sizeof(e131_packet_t));

    if (sent <= 0)
    {
        LOGERROR("Error sending sACN data");
    }
    senderPacket->frame.seq_number++;







}

void DMXSACNDevice::onContainerParameterChanged(Parameter* p)
{
    if (p == universeParam) {
        paramPacket();
    }
}

void DMXSACNDevice::paramPacket()
{
    if (universeParam->intValue() == 0)
    {
        LOGWARNING("Universe 0 is reserved for discovery, please use another one");
        return;
    }

    e131_pkt_init(&packet, universeParam->intValue(), 512);
}
