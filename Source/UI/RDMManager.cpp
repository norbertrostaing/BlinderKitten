#include <algorithm>
#include <cstring>
#include "RDMManager.h"
#include "Definitions/Interface/InterfaceIncludes.h"
#include "Definitions/Fixture/FixtureManager.h"
#include "Definitions/Fixture/FixturePatch.h"
#include "Definitions/Fixture/Fixture.h"
#include "Definitions/FixtureType/FixtureTypeManager.h"
#include "Definitions/FixtureType/FixtureType.h"
#include "Definitions/ChannelFamily/ChannelFamilyManager.h"
#include "Definitions/ChannelFamily/ChannelFamily.h"
#include "Definitions/ChannelFamily/ChannelType/ChannelType.h"

namespace
{
    const juce::Colour backgroundColour(0xff222222);
    const juce::Colour headerColour(0xff303030);
    juce::CriticalSection receiverListLock;
    RDMManager* currentRDMWindow = nullptr;

    uint16 read16(const uint8* p) { return uint16((uint16(p[0]) << 8) | p[1]); }
    uint32 read32(const uint8* p) { return (uint32(p[0]) << 24) | (uint32(p[1]) << 16) | (uint32(p[2]) << 8) | p[3]; }

    juce::String hexBytes(const uint8* p, int n)
    {
        juce::String s;
        for (int i = 0; i < n; ++i)
            s += juce::String::toHexString((int)p[i]).paddedLeft('0', 2).toUpperCase() + (i + 1 == n ? "" : " ");
        return s;
    }

    juce::String uidToString(const uint8* p)
    {
        return hexBytes(p, 2).removeCharacters(" ") + ":" + hexBytes(p + 2, 4).removeCharacters(" ");
    }

    juce::String pidName(int pid)
    {
        switch (pid)
        {
        case 0x0050: return "Supported PIDs";
        case 0x0051: return "PID description";
        case 0x0060: return "Device info";
        case 0x0080: return "Model";
        case 0x0081: return "Manufacturer";
        case 0x0082: return "Label";
        case 0x00c0: return "Firmware";
        case 0x00e0: return "DMX Mode";
        case 0x00e1: return "Personality list";
        case 0x00f0: return "DMX address";
        case 0x0120: return "Slot info";
        case 0x0121: return "Slot description";
        case 0x0122: return "Default slot values";
        case 0x0200: return "Sensor definition";
        case 0x0201: return "Sensor value";
        case 0x0400: return "Device hours";
        case 0x0401: return "Lamp hours";
        case 0x0402: return "Lamp strikes";
        case 0x0405: return "Power cycles";
        case 0x0500: return "Display invert";
        case 0x0501: return "Display level";
        case 0x0600: return "Pan invert";
        case 0x0601: return "Tilt invert";
        case 0x0602: return "Pan/Tilt swap";
        case 0x1000: return "Identify";
        default: return "PID 0x" + juce::String::toHexString(pid).paddedLeft('0', 4).toUpperCase();
        }
    }

    // E1.20 PIDs whose entire value is a one-byte Boolean (0 or 1).
    // DISPLAY_INVERT (0x0500) is an enum with an Auto state, not a Boolean.
    bool isBooleanPid(int pid)
    {
        return pid == 0x0600 || pid == 0x0601 || pid == 0x0602;
    }

    // Only parameters that have a human-readable, meaningful field in the grid.
    // The other PIDs (DEVICE_INFO, SUPPORTED_PARAMETERS, personality descriptions,
    // manufacturer-specific binary data...) remain available internally.
    bool isVisiblePid(int pid)
    {
        switch (pid)
        {
        case 0x0080: case 0x0081: case 0x0082: case 0x00c0:
        case 0x00e0: case 0x00f0:
        case 0x0201:
        case 0x0400: case 0x0401: case 0x0402: case 0x0405:
        case 0x0500: case 0x0501: case 0x0600: case 0x0601: case 0x0602:
            return true;
        default:
            return false;
        }
    }

    // A success ACK carrying raw bytes is not always useful text.
    bool hasUsableValue(int pid, const uint8* p, int n)
    {
        if (!isVisiblePid(pid)) return false;
        switch (pid)
        {
        case 0x0080: case 0x0081: case 0x0082: case 0x00c0:
            if (n <= 0 || n > 32) return false;
            for (int i = 0; i < n; ++i)
                if (p[i] < 32 || p[i] > 126) return false;
            return true;
        case 0x00e0: return n >= 2 && p[1] > 0;
        case 0x00f0: return n >= 2;
        case 0x0201: return n >= 3;
        case 0x0400: case 0x0401: case 0x0402: case 0x0405: return n == 4;
        case 0x0500: case 0x0501: return n == 1;
        case 0x0600: case 0x0601: case 0x0602: return n == 1 && p[0] <= 1;
        default: return false;
        }
    }

    juce::String decodeValue(int pid, const uint8* p, int n)
    {
        if (n == 0) return "(empty)";
        switch (pid)
        {
        case 0x0080: case 0x0081: case 0x0082: case 0x00c0:
            return juce::String::fromUTF8((const char*)p, n).trimEnd();
        case 0x00f0:
            if (n >= 2) return juce::String(read16(p));
            break;
        case 0x00e0:
            if (n >= 2) return juce::String(p[0]) + " / " + juce::String(p[1]);
            break;
        case 0x0060:
            if (n >= 19)
                return "Model " + juce::String(read16(p + 2)) + " | " + juce::String(read16(p + 10)) +
                    " ch | Mode " + juce::String(p[12]) + "/" + juce::String(p[13]) +
                    " | Addr " + juce::String(read16(p + 14)) + " | Sensors " + juce::String(p[18]);
            break;
        case 0x0400: case 0x0401: case 0x0402: case 0x0405:
            if (n >= 4) return juce::String((juce::int64)read32(p));
            break;
        case 0x0500: case 0x0501: case 0x0600: case 0x0601: case 0x0602: case 0x1000:
            if (n == 1) return juce::String(p[0]);
            break;
        case 0x0201:
            if (n >= 3) return "#" + juce::String(p[0]) + ": " + juce::String((int16)read16(p + 1));
            break;
        }
        return hexBytes(p, n);
    }
}

RDMManager::RDMManager() : ShapeShifterContentComponent("RDM Manager")
{
    addAndMakeVisible(scanButton);
    addAndMakeVisible(statusLabel);
    statusLabel.setText("Ready", juce::dontSendNotification);
    statusLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(table);
    table.setModel(this);
    table.setRowHeight(27);
    table.setHeaderHeight(27);
    addAndMakeVisible(actionsOverlay);
    table.setMultipleSelectionEnabled(false);
    table.setColour(juce::ListBox::backgroundColourId, backgroundColour);
    table.getHeader().setColour(juce::TableHeaderComponent::backgroundColourId, headerColour);
    setOpaque(true);
    scanButton.onClick = [this]() { startScan(); };

    // Controller UID: fixed ESTA-style controller prefix + per-window random serial.
    // For deployment use an assigned, stable manufacturer ID if available.
    controllerUID[0] = 0x7f;
    controllerUID[1] = 0xf0;
    uint32 serial = juce::Random::getSystemRandom().nextInt();
    controllerUID[2] = uint8(serial >> 24);
    controllerUID[3] = uint8(serial >> 16);
    controllerUID[4] = uint8(serial >> 8);
    controllerUID[5] = uint8(serial);

    // PID columns are added only when at least one fixture returns data.
    rebuildColumns();
    refreshOutputs();
    {
        const juce::ScopedLock lock(receiverListLock);
        currentRDMWindow = this;
    }
    startTimer(50);
}

RDMManager::~RDMManager()
{
    stopTimer();
    {
        const juce::ScopedLock lock(receiverListLock);
        if (currentRDMWindow == this) currentRDMWindow = nullptr;
    }
    table.setModel(nullptr);
}

void RDMManager::receiveArtNet(const uint8* bytes, int length, const juce::String& senderIP)
{
    if (length < 12 || std::memcmp(bytes, "Art-Net\0", 8) != 0) return;
    int opcode = bytes[8] | (bytes[9] << 8);
    if (opcode != 0x8100 && opcode != 0x8300) return;
    const juce::ScopedLock receivers(receiverListLock);
    if (currentRDMWindow == nullptr) return;
    const juce::ScopedLock queueLock(currentRDMWindow->receiveLock);
    if (currentRDMWindow->receivedPackets.size() >= 256) return;
    Packet packet;
    packet.data.append(bytes, (size_t)length);
    packet.senderIP = senderIP;
    currentRDMWindow->receivedPackets.push_back(std::move(packet));
}

void RDMManager::paint(juce::Graphics& g)
{
    g.fillAll(backgroundColour);
}

void RDMManager::resized()
{
    juce::Rectangle<int> area = getLocalBounds().reduced(4);
    juce::Rectangle<int> top = area.removeFromTop(28);
    scanButton.setBounds(top.removeFromLeft(140).reduced(2));
    statusLabel.setBounds(top);
    area.removeFromTop(4);
    actionsOverlay.setBounds(area.removeFromRight(48));
    table.setBounds(area);
    actionsOverlay.toFront(false);
    actionsOverlay.repaint();
}

void RDMManager::refreshOutputs()
{
    juce::Array<DMXInterface*> newOutputs;
    for (auto& item : InterfaceManager::getInstance()->items)
    {
        DMXInterface* out = dynamic_cast<DMXInterface*>(item);
        if (out != nullptr && out->enabled->boolValue() &&
            dynamic_cast<DMXArtNetDevice*>(out->dmxDevice.get()) != nullptr)
            newOutputs.add(out);
    }
    if (newOutputs == outputs) return;
    outputs = newOutputs;
    resetScan();
}

void RDMManager::resetScan()
{
    devices.clear();
    { const juce::ScopedLock lock(receiveLock); receivedPackets.clear(); }
    requests.clear();
    hasActiveRequest = false;
    scanActive = false;
    scanEverStarted = false;
    columns.clear();
    rebuildColumns();
    statusLabel.setText("Ready", juce::dontSendNotification);
    table.updateContent();
    actionsOverlay.repaint();
}

bool RDMManager::sendArtNet(DMXInterface* output, const juce::String& ip, const uint8* data, int size)
{
    if (!outputs.contains(output) || ip.isEmpty()) return false;
    DMXArtNetDevice* dev = dynamic_cast<DMXArtNetDevice*>(output->dmxDevice.get());
    if (dev == nullptr) return false;
    ArtnetSocket* transport = ArtnetSocket::getInstance();
    const juce::ScopedLock lock(transport->isSending);
    if (transport->socket == nullptr) return false;
    return transport->socket->write(ip, dev->remotePort->intValue(), data, size) == size;
}

void RDMManager::startScan()
{
    refreshOutputs();
    resetScan();
    if (outputs.isEmpty())
    {
        statusLabel.setText("No enabled Art-Net outputs", juce::dontSendNotification);
        return;
    }

    // Use the existing shared Art-Net socket for all outputs and responses.
    // Set this only once; the existing DMXArtNetDevice code rebinds to 6454.
    DMXArtNetDevice* first = dynamic_cast<DMXArtNetDevice*>(outputs[0]->dmxDevice.get());
    if (first != nullptr && !first->forceSrcPort->boolValue())
        first->forceSrcPort->setValue(true);

    scanActive = true;
    scanEverStarted = true;
    scanStarted = juce::Time::getMillisecondCounterHiRes();
    lastTodRequest = 0;

    for (DMXInterface* output : outputs)
        sendTodControl(output);

    statusLabel.setText("Discovering on " + juce::String(outputs.size()) +
        " Art-Net outputs...", juce::dontSendNotification);
}

void RDMManager::sendTodControl(DMXInterface* output)
{
    DMXArtNetDevice* dev = dynamic_cast<DMXArtNetDevice*>(output->dmxDevice.get());
    if (dev == nullptr) return;
    uint8 p[24] = { 'A','r','t','-','N','e','t',0, 0x00, 0x82, 0, 14 };
    p[21] = uint8(dev->outputNet->intValue());
    p[22] = 1; // AtcFlush
    p[23] = uint8((dev->outputSubnet->intValue() << 4) | dev->outputUniverse->intValue());
    sendArtNet(output, dev->remoteHost->stringValue(), p, sizeof(p));
}

void RDMManager::sendTodRequest(DMXInterface* output)
{
    DMXArtNetDevice* dev = dynamic_cast<DMXArtNetDevice*>(output->dmxDevice.get());
    if (dev == nullptr) return;
    uint8 p[56] = { 'A','r','t','-','N','e','t',0, 0x00, 0x80, 0, 14 };
    p[21] = uint8(dev->outputNet->intValue());
    p[22] = 0; // TodFull
    p[23] = 1;
    p[24] = uint8((dev->outputSubnet->intValue() << 4) | dev->outputUniverse->intValue());
    sendArtNet(output, dev->remoteHost->stringValue(), p, sizeof(p));
}

void RDMManager::queueRdm(int index, int pid, bool set, const juce::MemoryBlock& payload)
{
    if (index < 0 || index >= devices.size()) return;
    Device& device = devices.getReference(index);
    if (!set && payload.getSize() == 0 && !device.requested.insert(pid).second) return;
    if (!set && payload.getSize() != 0) device.requested.insert(pid);
    Request request;
    request.uid = device.uid;
    request.uidText = device.uidText;
    request.nodeIP = device.nodeIP;
    request.output = device.output;
    request.net = device.net;
    request.addressLow = device.addressLow;
    request.pid = pid;
    request.set = set;
    request.payload = payload;
    requests.push_back(std::move(request));
}

void RDMManager::sendRdm(Request& request)
{
    if (!outputs.contains(request.output)) return;
    DMXArtNetDevice* dev = dynamic_cast<DMXArtNetDevice*>(request.output->dmxDevice.get());
    if (dev == nullptr) return;
    std::vector<uint8> rdm;
    rdm.reserve(256);
    rdm.push_back(0x01); // RDM Sub Start Code (start code 0xCC is omitted in ArtRdm)
    rdm.push_back(uint8(24 + request.payload.getSize())); // RDM message length EXCLUDING checksum
    for (uint8 b : request.uid) rdm.push_back(b);
    for (uint8 b : controllerUID) rdm.push_back(b);
    rdm.push_back(request.transaction);
    rdm.push_back(0x01); // Port ID
    rdm.push_back(0x00); // Message count
    rdm.push_back(0x00); rdm.push_back(0x00); // Root subdevice
    rdm.push_back(request.set ? 0x30 : 0x20);
    rdm.push_back(uint8(request.pid >> 8));
    rdm.push_back(uint8(request.pid));
    rdm.push_back(uint8(request.payload.getSize()));
    const uint8* body = (const uint8*)request.payload.getData();
    for (size_t i = 0; i < request.payload.getSize(); ++i) rdm.push_back(body[i]);
    uint16 checksum = 0xcc;
    for (uint8 b : rdm) checksum += b;
    rdm.push_back(uint8(checksum >> 8));
    rdm.push_back(uint8(checksum));

    std::vector<uint8> packet(24 + rdm.size(), 0);
    const uint8 header[12] = { 'A','r','t','-','N','e','t',0, 0x00, 0x83, 0, 14 };
    std::memcpy(packet.data(), header, sizeof(header));
    packet[12] = 0x01; // Standard RDM
    packet[21] = uint8(request.net);
    packet[22] = 0; // ArProcess
    packet[23] = uint8(request.addressLow);
    std::memcpy(packet.data() + 24, rdm.data(), rdm.size());
    sendArtNet(request.output, request.nodeIP, packet.data(), (int)packet.size());
    request.sentAt = juce::Time::getMillisecondCounterHiRes();
}

void RDMManager::timerCallback()
{
    double now = juce::Time::getMillisecondCounterHiRes();
    refreshOutputs();
    std::deque<Packet> packets;
    {
        const juce::ScopedLock lock(receiveLock);
        packets.swap(receivedPackets);
    }
    for (const Packet& p : packets) processPacket(p);
    if (auto* vp = table.getViewport())
    {
        const int scrollY = vp->getViewPositionY();
        if (scrollY != lastTableScrollY)
        {
            lastTableScrollY = scrollY;
            actionsOverlay.repaint();
        }
    }

    if (scanActive)
    {
        if (now - scanStarted > 30000) scanActive = false;
        else if (now - scanStarted > 650 && now - lastTodRequest > 1600)
        {
            for (DMXInterface* output : outputs)
                sendTodRequest(output);
            lastTodRequest = now;
        }
    }

    if (hasActiveRequest && now - activeRequest.sentAt > 700)
    {
        if (activeRequest.retries < 1)
        {
            ++activeRequest.retries;
            sendRdm(activeRequest);
        }
        else
        {
            // Do not create a column for unanswered requests.
            if (activeRequest.pid == 0x0120)
            {
                int index = findDevice(activeRequest.uidText, activeRequest.nodeIP,
                                       activeRequest.net, activeRequest.addressLow);
                if (index >= 0) devices.getReference(index).slotInfoUnavailable = true;
            }
            hasActiveRequest = false;
            table.updateContent(); // Restores a dropdown after an unanswered SET.
        }
    }
    if (!hasActiveRequest && !requests.empty())
    {
        activeRequest = requests.front();
        requests.pop_front();
        activeRequest.transaction = ++transactionNumber;
        activeRequest.retries = 0;
        hasActiveRequest = true;
        sendRdm(activeRequest);
    }
    if (devices.size() > 0)
        statusLabel.setText(juce::String(devices.size()) + " devices / " + juce::String(outputs.size()) +
            " Art-Net outputs | " + juce::String(requests.size() + (hasActiveRequest ? 1 : 0)) +
            " requests remaining" + (scanActive ? " | Discovering..." : ""), juce::dontSendNotification);
    else if (!scanActive && scanEverStarted && !outputs.isEmpty())
        statusLabel.setText("No RDM devices returned", juce::dontSendNotification);
}

void RDMManager::processPacket(const Packet& packet)
{
    const uint8* p = (const uint8*)packet.data.getData();
    int size = (int)packet.data.getSize();
    if (size < 12) return;
    int opcode = p[8] | (p[9] << 8);
    if (opcode == 0x8100) processTod(p, size, packet.senderIP);
    else if (opcode == 0x8300) processRdm(p, size, packet.senderIP);
}

int RDMManager::findDevice(const juce::String& uid, const juce::String& ip,
                           int net, int addressLow) const
{
    for (int i = 0; i < devices.size(); ++i)
    {
        const Device& d = devices.getReference(i);
        if (d.uidText == uid && d.nodeIP == ip && d.net == net && d.addressLow == addressLow)
            return i;
    }
    return -1;
}

void RDMManager::processTod(const uint8* p, int size, const juce::String& senderIP)
{
    if (size < 28 || p[22] != 0 || size < 28 + 6 * p[27]) return;

    DMXInterface* matchingOutput = nullptr;
    DMXInterface* broadcastOutput = nullptr;
    for (DMXInterface* output : outputs)
    {
        DMXArtNetDevice* dev = dynamic_cast<DMXArtNetDevice*>(output->dmxDevice.get());
        if (dev == nullptr) continue;
        int addressLow = (dev->outputSubnet->intValue() << 4) | dev->outputUniverse->intValue();
        if (p[21] != dev->outputNet->intValue() || p[23] != addressLow) continue;
        juce::String host = dev->remoteHost->stringValue();
        if (host == senderIP) { matchingOutput = output; break; }
        if (host.endsWith(".255") || host == "255.255.255.255")
            broadcastOutput = output;
    }
    // Prefer an exact configured IP. Otherwise accept a directed-broadcast output.
    if (matchingOutput == nullptr) matchingOutput = broadcastOutput;
    if (matchingOutput == nullptr) return;

    for (int i = 0; i < p[27]; ++i)
    {
        const uint8* uid = p + 28 + i * 6;
        juce::String id = uidToString(uid);
        if (findDevice(id, senderIP, p[21], p[23]) >= 0) continue;
        Device device;
        std::memcpy(device.uid.data(), uid, 6);
        device.uidText = id;
        device.nodeIP = senderIP;
        device.output = matchingOutput;
        device.net = p[21];
        device.addressLow = p[23];
        auto assigned = associations.find(deviceKey(device));
        if (assigned != associations.end())
            device.associatedFixtureId = assigned->second;
        devices.add(device);
        int index = devices.size() - 1;
        const int initialPids[] = { 0x0050, 0x0060, 0x0080, 0x0081, 0x0082, 0x00c0, 0x00e0, 0x00f0, 0x0400, 0x0401 };
        for (int pid : initialPids) queueRdm(index, pid);
    }
    table.updateContent();
    actionsOverlay.repaint();
}

void RDMManager::processRdm(const uint8* p, int size, const juce::String& senderIP)
{
    if (!hasActiveRequest || !outputs.contains(activeRequest.output) || size < 49) return;
    DMXArtNetDevice* dev = dynamic_cast<DMXArtNetDevice*>(activeRequest.output->dmxDevice.get());
    if (dev == nullptr) return;
    if (p[21] != activeRequest.net || p[23] != activeRequest.addressLow) return;
    if (senderIP != activeRequest.nodeIP) return;
    const uint8* rdm = p + 24;
    int length = size - 24;
    if (rdm[0] != 0x01 || rdm[1] < 24 || length < int(rdm[1]) + 1) return;
    int pdl = rdm[22];
    if (rdm[1] != 24 + pdl || length < 25 + pdl) return;
    if (rdm[14] != activeRequest.transaction || read16(rdm + 20) != activeRequest.pid) return;
    if (rdm[19] != (activeRequest.set ? 0x31 : 0x21)) return;
    if (std::memcmp(rdm + 8, activeRequest.uid.data(), 6) != 0) return;
    if (std::memcmp(rdm + 2, controllerUID.data(), 6) != 0) return;
    uint16 checksum = 0xcc;
    for (int i = 0; i < 23 + pdl; ++i) checksum += rdm[i];
    if (checksum != read16(rdm + 23 + pdl)) return;

    int index = findDevice(activeRequest.uidText, senderIP, p[21], p[23]);
    if (index < 0) return;
    Device& device = devices.getReference(index);
    int pid = activeRequest.pid;
    bool wasSet = activeRequest.set;
    const uint8 responseType = rdm[15];
    if (responseType == 0x01) // ACK_TIMER: no data yet
    {
        table.repaint();
        return;
    }
    if (responseType == 0x02) // NACK_REASON
    {
        if (pid == 0x0120) device.slotInfoUnavailable = true;
        hasActiveRequest = false;
        table.updateContent();
        return;
    }
    if (responseType != 0x00 && responseType != 0x03)
    {
        if (pid == 0x0120) device.slotInfoUnavailable = true;
        hasActiveRequest = false;
        return;
    }

    const uint8* data = rdm + 23;
    int dataLength = pdl;
    // RDM E1.20 ACK_OVERFLOW: the next GET must target the same PID
    // without another PID interrupting the sequence on that responder.
    if (!wasSet && responseType == 0x03)
    {
        if (++activeRequest.overflowCount > 64 ||
            activeRequest.overflowData.getSize() + (size_t)dataLength > 16384)
        {
            if (pid == 0x0120) device.slotInfoUnavailable = true;
            hasActiveRequest = false;
            return;
        }
        activeRequest.overflowData.append(data, (size_t)dataLength);
        activeRequest.transaction = ++transactionNumber;
        activeRequest.retries = 0;
        sendRdm(activeRequest);
        return;
    }
    if (!wasSet && activeRequest.overflowData.getSize() > 0)
    {
        activeRequest.overflowData.append(data, (size_t)dataLength);
        data = (const uint8*)activeRequest.overflowData.getData();
        dataLength = (int)activeRequest.overflowData.getSize();
    }

    hasActiveRequest = false;
    if (wasSet)
    {
        if (pid == 0x1000 && activeRequest.payload.getSize() == 1)
            device.identify = *(const uint8*)activeRequest.payload.getData() != 0;
        device.requested.erase(pid);
        queueRdm(index, pid); // confirm SET by reading back
        return;
    }
    if (pid == 0x0050)
    {
        device.supportedPidsRead = true;
        for (int i = 0; i + 1 < dataLength; i += 2)
        {
            int supportedPid = read16(data + i);
            if (supportedPid == 0x0120) device.supportsSlotInfo = true;
            if (supportedPid == 0x0121) device.supportsSlotDescription = true;
            if (supportedPid == 0x0122) device.supportsDefaultSlots = true;
            if (isVisiblePid(supportedPid) && supportedPid != 0x0201 &&
                device.requested.size() < 160)
                queueRdm(index, supportedPid);
        }
        // Slot PIDs are queried on demand from the ... menu, not as columns.
    }
    else if (pid == 0x0120 && dataLength > 0 && dataLength % 5 == 0)
    {
        device.slots.clear();
        for (int i = 0; i + 4 < dataLength; i += 5)
        {
            const int offset = read16(data + i);
            if (offset < 0 || offset >= 512) continue;
            Slot& slot = device.slots[offset];
            slot.offset = offset;
            slot.type = data[i + 2];
            slot.label = read16(data + i + 3);
        }
        device.slotInfoRead = true;
        device.slotInfoUnavailable = false;
        // Request individual descriptions only where the standard says this is
        // a primary channel; a secondary slot's label field is its parent's OFFSET.
        if (device.supportsSlotDescription)
        {
            for (const auto& entry : device.slots)
            {
                if (entry.second.type != 0) continue;
                uint8 offset[2] = { uint8(entry.first >> 8), uint8(entry.first) };
                queueRdm(index, 0x0121, false, juce::MemoryBlock(offset, 2));
            }
        }
    }
    else if (pid == 0x0120)
    {
        // A zero-length or non-multiple-of-five slot map is not usable.
        device.slotInfoUnavailable = true;
    }
    else if (pid == 0x0121 && dataLength >= 2)
    {
        int offset = read16(data);
        if (offset < 512)
            device.slots[offset].description =
                juce::String::fromUTF8((const char*)data + 2, dataLength - 2).trim();
    }
    else if (pid == 0x0122 && dataLength % 3 == 0)
    {
        for (int i = 0; i + 2 < dataLength; i += 3)
        {
            int offset = read16(data + i);
            if (offset >= 0 && offset < 512)
            {
                device.slots[offset].offset = offset;
                device.slots[offset].defaultValue = data[i + 2];
            }
        }
    }
    else if (pid == 0x00e1 && dataLength >= 3)
    {
        // E1.20: personality number, footprint (2 bytes), then ASCII description.
        const int number = data[0];
        const juce::String description = juce::String::fromUTF8((const char*)(data + 3), dataLength - 3).trim();
        const int modeFootprint = read16(data + 1);
        device.personalityDescriptions[number] = description +
            " (" + juce::String(modeFootprint) + " ch)";
        if (number == device.personality && modeFootprint >= 1 && modeFootprint <= 512)
            device.footprint = modeFootprint;
    }
    else if (hasUsableValue(pid, data, dataLength))
    {
        if (pid == 0x0201)
        {
            if (device.values[pid].isNotEmpty()) device.values[pid] += "; ";
            device.values[pid] += decodeValue(pid, data, dataLength);
        }
        else device.values[pid] = decodeValue(pid, data, dataLength);
    }
    if (pid == 0x00f0 && dataLength >= 2) device.address = read16(data);
    if (pid == 0x00e0 && dataLength >= 2)
    {
        if (device.personality != 0 && device.personality != data[0])
        {
            device.slots.clear();
            device.slotInfoRead = false;
            device.slotInfoRequested = false;
            device.slotInfoUnavailable = false;
            device.requested.erase(0x0120);
            device.requested.erase(0x0122);
        }
        device.personality = data[0];
        device.personalityCount = data[1];
        // Fetch every reported mode description, not just the first 16.
        for (int mode = 1; mode <= device.personalityCount; ++mode)
        {
            if (device.personalityDescriptions.find(mode) != device.personalityDescriptions.end())
                continue;
            uint8 personality = uint8(mode);
            juce::MemoryBlock payload(&personality, 1);
            queueRdm(index, 0x00e1, false, payload);
        }
    }
    if (pid == 0x0060 && dataLength >= 19)
    {
        if (device.personality != 0 && device.personality != data[12])
        {
            device.slots.clear();
            device.slotInfoRead = false;
            device.slotInfoRequested = false;
            device.slotInfoUnavailable = false;
            device.requested.erase(0x0120);
            device.requested.erase(0x0122);
        }
        device.personality = data[12];
        device.personalityCount = data[13];
        device.footprint = read16(data + 10);
        device.address = read16(data + 14);
        for (int sensor = 0; sensor < juce::jmin((int)data[18], 16); ++sensor)
        {
            uint8 sensorNumber = uint8(sensor);
            juce::MemoryBlock payload(&sensorNumber, 1);
            queueRdm(index, 0x0201, false, payload);
        }
    }
    auto value = device.values.find(pid);
    if (isVisiblePid(pid) && value != device.values.end() && value->second.isNotEmpty())
        addColumn(pid);
    if (pid == 0x00e0 || pid == 0x00e1 || isBooleanPid(pid))
        table.updateContent(); // Refresh the controls from the confirmed GET values.
    if (pid == 0x00f0 || pid == 0x0060)
        rebuildColumns(); // The patch association may now be known.
    table.repaint();
}

void RDMManager::addColumn(int pid)
{
    if (std::find(columns.begin(), columns.end(), pid) != columns.end()) return;
    columns.push_back(pid);
    rebuildColumns();
}

void RDMManager::rebuildColumns()
{
    juce::TableHeaderComponent& header = table.getHeader();
    header.removeAllColumns();
    header.addColumn("UID", 1, 115, 100, 200);
    // Only explicitly linked AND effectively patched BK fixtures count.
    bool hasFixture = false;
    for (const Device& device : devices)
    {
        if (getFixtureName(device).isNotEmpty())
        {
            hasFixture = true;
            break;
        }
    }
    if (hasFixture) header.addColumn("Fixture BK", 2, 145, 80, 300);
    header.addColumn("Interface", 5, 145, 90, 260);
    header.addColumn("Universe", 6, 105, 90, 150);
    header.addColumn("Node IP", 3, 120, 80, 190);
    for (int pid : columns)
        if (isVisiblePid(pid))
            header.addColumn(pidName(pid), pid + 1000, pid == 0x00e0 ? 225 : 150, 60, 400);
    table.updateContent();
    actionsOverlay.repaint();
}

int RDMManager::getNumRows() { return devices.size(); }

std::string RDMManager::deviceKey(const Device& device) const
{
    return (device.uidText + "|" + device.nodeIP + "|" + juce::String(device.net) +
            "|" + juce::String(device.addressLow)).toStdString();
}

juce::String RDMManager::getFixtureName(const Device& device) const
{
    // Never guess from DMX address: two unrelated fixtures could share that address.
    if (device.associatedFixtureId <= 0 || device.output == nullptr || device.address < 1)
        return {};
    for (Fixture* fixture : FixtureManager::getInstance()->items)
    {
        if (fixture->id->intValue() != device.associatedFixtureId) continue;
        for (FixturePatch* patch : fixture->patchs.items)
        {
            if (patch->enabled->boolValue() &&
                patch->targetInterface->targetContainer.get() == device.output &&
                patch->currentInterface->targetContainer.get() == device.output &&
                patch->address->intValue() == device.address &&
                patch->currentAdresses.contains(device.address))
                return fixture->id->stringValue() + " " + fixture->userName->stringValue();
        }
    }
    return {};
}

juce::String RDMManager::cellText(int row, int columnId) const
{
    if (row < 0 || row >= devices.size()) return {};
    const Device& device = devices.getReference(row);
    if (columnId == 1) return device.uidText;
    if (columnId == 2) return getFixtureName(device);
    if (columnId == 3) return device.nodeIP;
    if (columnId == 5) return device.output != nullptr ? device.output->niceName : "—";
    if (columnId == 6) return juce::String(device.net) + " / " +
        juce::String(device.addressLow >> 4) + " / " + juce::String(device.addressLow & 15);
    if (columnId == 4) return {};
    int pid = columnId - 1000;
    auto it = device.values.find(pid);
    return it == device.values.end() ? juce::String() : it->second;
}

void RDMManager::paintRowBackground(juce::Graphics& g, int row, int width, int height, bool selected)
{
    g.fillAll(selected ? juce::Colour(0xff303c48) : backgroundColour);
}

void RDMManager::paintCell(juce::Graphics& g, int row, int columnId, int width, int height, bool)
{
    int pid = columnId - 1000;
    bool editable = pid == 0x0082 || pid == 0x00e0 || pid == 0x00f0 ||
                    pid == 0x0500 || pid == 0x0501 || isBooleanPid(pid);
    if (!editable)
    {
        g.setColour(juce::Colour(0xff292929));
        g.fillRect(0, 0, width, height);
    }
    g.setColour(juce::Colours::white.withAlpha(editable ? 0.95f : 0.52f));
    g.setFont(12.0f);
    g.drawFittedText(cellText(row, columnId), 5, 0, width - 9, height, juce::Justification::centredLeft, 1);
    g.setColour(juce::Colours::white.withAlpha(0.09f));
    g.drawVerticalLine(width - 1, 0.0f, (float)height);
}

void RDMManager::cellClicked(int, int, const juce::MouseEvent&) {}

void RDMManager::cellDoubleClicked(int row, int columnId, const juce::MouseEvent&)
{
    if (columnId != 0x00e0 + 1000) editCell(row, columnId);
}

juce::Component* RDMManager::refreshComponentForCell(int row, int columnId, bool,
                                                     juce::Component* existing)
{
    if (row < 0 || row >= devices.size())
    {
        delete existing;
        return nullptr;
    }

    const int pid = columnId - 1000;
    if (isBooleanPid(pid))
    {
        const Device& device = devices.getReference(row);
        auto value = device.values.find(pid);
        if (value == device.values.end() ||
            (value->second != "0" && value->second != "1"))
        {
            delete existing;
            return nullptr;
        }

        juce::ToggleButton* checkbox = dynamic_cast<juce::ToggleButton*>(existing);
        if (checkbox == nullptr)
        {
            delete existing;
            checkbox = new juce::ToggleButton();
        }
        checkbox->onClick = nullptr;
        checkbox->setToggleState(value->second == "1", juce::dontSendNotification);
        checkbox->setTooltip(pidName(pid));
        checkbox->setColour(juce::ToggleButton::textColourId, juce::Colours::white);

        const juce::String uid = device.uidText, ip = device.nodeIP;
        const int net = device.net, port = device.addressLow;
        juce::Component::SafePointer<RDMManager> safe(this);
        checkbox->onClick = [safe, checkbox, uid, ip, net, port, pid]()
        {
            if (safe == nullptr) return;
            const int index = safe->findDevice(uid, ip, net, port);
            if (index < 0) return;
            const uint8 state = checkbox->getToggleState() ? 1 : 0;
            const juce::MemoryBlock data(&state, 1);
            safe->queueRdm(index, pid, true, data);
        };
        return checkbox;
    }

    if (columnId != 0x00e0 + 1000 || devices[row].personalityCount == 0)
    {
        delete existing;
        return nullptr;
    }

    juce::ComboBox* combo = dynamic_cast<juce::ComboBox*>(existing);
    if (combo == nullptr)
    {
        delete existing;
        combo = new juce::ComboBox();
    }
    combo->onChange = nullptr;
    combo->setColour(juce::ComboBox::backgroundColourId, backgroundColour);
    combo->setColour(juce::ComboBox::textColourId, juce::Colours::white);
    combo->clear(juce::dontSendNotification);
    const Device& device = devices.getReference(row);
    for (int number = 1; number <= device.personalityCount; ++number)
    {
        juce::String label = juce::String(number);
        auto description = device.personalityDescriptions.find(number);
        if (description != device.personalityDescriptions.end())
            label += " - " + description->second;
        combo->addItem(label, number);
    }
    combo->setSelectedId(device.personality, juce::dontSendNotification);
    const juce::String uid = device.uidText, ip = device.nodeIP;
    const int net = device.net, port = device.addressLow;
    juce::Component::SafePointer<RDMManager> safe(this);
    combo->onChange = [safe, combo, uid, ip, net, port]()
    {
        if (safe == nullptr) return;
        const int index = safe->findDevice(uid, ip, net, port);
        if (index < 0) return;
        const int mode = combo->getSelectedId();
        if (mode < 1 || mode > safe->devices[index].personalityCount ||
            mode == safe->devices[index].personality) return;
        const uint8 b = uint8(mode);
        juce::MemoryBlock payload(&b, 1);
        safe->queueRdm(index, 0x00e0, true, payload);
    };
    return combo;
}

void RDMManager::editCell(int row, int columnId)
{
    if (row < 0 || row >= devices.size()) return;
    int pid = columnId - 1000;
    if (pid != 0x0082 && pid != 0x00f0 && pid != 0x0500 && pid != 0x0501) return;
    juce::String current = cellText(row, columnId);
    juce::AlertWindow* window = new juce::AlertWindow("RDM SET " + pidName(pid),
        "New value for " + devices[row].uidText, juce::AlertWindow::QuestionIcon);
    window->addTextEditor("value", current, "Value");
    window->addButton("Send", 1);
    window->addButton("Cancel", 0);
    juce::Component::SafePointer<RDMManager> safe(this);
    juce::Component::SafePointer<juce::AlertWindow> safeWindow(window);
    window->enterModalState(true, juce::ModalCallbackFunction::create([safe, safeWindow, row, pid, current](int result)
    {
        if (result != 1 || safe == nullptr || safeWindow == nullptr || row >= safe->devices.size()) return;
        juce::String value = safeWindow->getTextEditorContents("value").trim();
        if (value.isEmpty() || value == current) return;
        juce::MemoryBlock data;
        if (pid == 0x0082)
        {
            juce::String label = value.substring(0, 32);
            while (label.getNumBytesAsUTF8() > 32) label = label.dropLastCharacters(1);
            data.append(label.toRawUTF8(), label.getNumBytesAsUTF8());
        }
        else
        {
            if (!value.containsOnly("0123456789")) return;
            int number = value.getIntValue();
            if (pid == 0x00f0)
            {
                if (number < 1 || number > 512) return;
                uint8 bytes[] = { uint8(number >> 8), uint8(number) };
                data.append(bytes, 2);
            }
            else
            {
                if (number < 0 || number > 255) return;
                uint8 b = uint8(number);
                data.append(&b, 1);
            }
        }
        safe->queueRdm(row, pid, true, data);
    }), true);
}

void RDMManager::showDeviceMenu(int row)
{
    if (row < 0 || row >= devices.size()) return;
    juce::PopupMenu menu;
    menu.addItem(1, devices[row].identify ? "Stop identifying" : "Identify / Flash");
    menu.addItem(2, "Refresh parameters");
    menu.addItem(3, "DMX Channels...");
    menu.addItem(4, "Create Fixture Type from RDM",
        devices[row].footprint >= 1 && devices[row].footprint <= 512);
    juce::PopupMenu fixtures;
    int n = 0;
    for (Fixture* fixture : FixtureManager::getInstance()->items)
        fixtures.addItem(1000 + n++, fixture->id->stringValue() + " - " + fixture->userName->stringValue());
    menu.addSubMenu("Use for BK fixture", fixtures, n > 0 && devices[row].address > 0);

    juce::PopupMenu fixtureTypes;
    for (int i = 0; i < FixtureTypeManager::getInstance()->items.size(); ++i)
    {
        FixtureType* type = FixtureTypeManager::getInstance()->items[i];
        fixtureTypes.addItem(10000 + i, type->niceName);
    }
    menu.addSubMenu("Create BK fixture", fixtureTypes,
        devices[row].address > 0 && devices[row].output != nullptr &&
        !FixtureTypeManager::getInstance()->items.isEmpty());

    const Device& device = devices.getReference(row);
    const juce::String uid = device.uidText, ip = device.nodeIP;
    const int net = device.net, port = device.addressLow;
    juce::Component::SafePointer<RDMManager> safe(this);
    menu.showMenuAsync(juce::PopupMenu::Options(), [safe, uid, ip, net, port](int result)
    {
        if (safe == nullptr || result == 0) return;
        const int row = safe->findDevice(uid, ip, net, port);
        if (row < 0) return;
        if (result == 1)
        {
            uint8 on = safe->devices[row].identify ? 0 : 1;
            juce::MemoryBlock payload(&on, 1);
            safe->queueRdm(row, 0x1000, true, payload);
        }
        else if (result == 2)
        {
            Device& device = safe->devices.getReference(row);
            device.requested.clear();
            const int pids[] = { 0x0050,0x0060,0x0080,0x0081,0x0082,0x00c0,0x00e0,0x00f0,0x0400,0x0401 };
            for (int pid : pids) safe->queueRdm(row, pid);
        }
        else if (result == 3) safe->showSlots(row);
        else if (result == 4) safe->showSlots(row); // Preview before creating the type
        else if (result >= 10000) safe->createFixture(row, result - 10000);
        else if (result >= 1000) safe->associateFixture(row, result - 1000);
    });
}

void RDMManager::createFixture(int row, int fixtureTypeIndex)
{
    if (row < 0 || row >= devices.size()) return;
    if (fixtureTypeIndex < 0 ||
        fixtureTypeIndex >= FixtureTypeManager::getInstance()->items.size()) return;

    // Keep an independent copy before addItem()/setValueFromTarget(), which
    // can trigger BK callbacks and invalidate references into devices.
    const Device device = devices.getReference(row);
    if (device.address < 1 || device.address > 512 ||
        !outputs.contains(device.output)) return;

    auto labelIt = device.values.find(0x0082);
    const juce::String label = labelIt != device.values.end() ? labelIt->second : juce::String();
    FixtureType* type = FixtureTypeManager::getInstance()->items[fixtureTypeIndex];

    Fixture* fixture = FixtureManager::getInstance()->addItem();
    if (fixture == nullptr) return;
    fixture->devTypeParam->setValueFromTarget(type);

    if (label.isNotEmpty())
        fixture->userName->setValue(label);

    // A callback may have re-ordered or refreshed the RDM list.
    const int index = FixtureManager::getInstance()->items.indexOf(fixture);
    const int currentRow = findDevice(device.uidText, device.nodeIP, device.net, device.addressLow);
    if (currentRow >= 0 && index >= 0)
        associateFixture(currentRow, index);
}

void RDMManager::associateFixture(int row, int fixtureIndex)
{
    if (row < 0 || row >= devices.size()) return;
    DMXInterface* out = devices.getReference(row).output;
    if (out == nullptr || !outputs.contains(out)) return;
    if (fixtureIndex < 0 || fixtureIndex >= FixtureManager::getInstance()->items.size()) return;
    Fixture* fixture = FixtureManager::getInstance()->items[fixtureIndex];
    if (devices[row].address < 1 || devices[row].address > 512) return;
    FixturePatch* patch = fixture->patchs.items.isEmpty() ? fixture->patchs.addItem() : fixture->patchs.items[0];
    if (patch == nullptr) return;
    patch->targetInterface->setValueFromTarget(out);
    patch->address->setValue(devices[row].address);
    if (!patch->enabled->boolValue())
        patch->enabled->setValue(true);
    if (!patch->enabled->boolValue() ||
        patch->currentInterface->targetContainer.get() != out ||
        !patch->currentAdresses.contains(devices[row].address))
    {
        statusLabel.setText("BK patch not applied (check fixture type / address conflict)",
                            juce::dontSendNotification);
        return;
    }
    devices.getReference(row).associatedFixtureId = fixture->id->intValue();
    associations[deviceKey(devices.getReference(row))] = fixture->id->intValue();
    rebuildColumns();
    table.repaint();
}


// RDM E1.20, Annex C. Unknown manufacturer slots remain unnamed.
juce::String RDMManager::slotLabelName(int id)
{
    switch (id)
    {
    case 0x0001: return "Intensity";
    case 0x0002: return "Intensity Master";
    case 0x0101: return "Pan";
    case 0x0102: return "Tilt";
    case 0x0201: return "Color Wheel";
    case 0x0202: return "Cyan";
    case 0x0203: return "Yellow";
    case 0x0204: return "Magenta";
    case 0x0205: return "Red";
    case 0x0206: return "Green";
    case 0x0207: return "Blue";
    case 0x0208: return "Color Correction";
    case 0x0209: return "Color Scroll";
    case 0x0211: return "Amber";
    case 0x0301: return "Static Gobo Wheel";
    case 0x0302: return "Rotating Gobo Wheel";
    case 0x0303: return "Prism";
    case 0x0304: return "Effects Wheel";
    case 0x0401: return "Iris";
    case 0x0402: return "Focus";
    case 0x0403: return "Frost";
    case 0x0404: return "Shutter / Strobe";
    case 0x0405: return "Zoom";
    case 0x0406: return "Framing Shutter";
    case 0x0407: return "Shutter Rotation";
    case 0x0408: return "Douser";
    case 0x0409: return "Barn Door";
    case 0x0501: return "Lamp Control";
    case 0x0502: return "Fixture Control";
    case 0x0503: return "Fixture Speed";
    case 0x0504: return "Macro";
    }
    if (id == 0xffff) return "Unknown";
    return "Slot ID 0x" + juce::String::toHexString(id).paddedLeft('0', 4).toUpperCase();
}

juce::String RDMManager::slotTypeName(int type)
{
    switch (type)
    {
    case 0: return "Primary";
    case 1: return "Fine";
    case 2: return "Timing";
    case 3: return "Speed";
    case 4: return "Control";
    case 5: return "Index";
    case 6: return "Rotation";
    case 7: return "Index / Rotate";
    case 255: return "Undefined secondary";
    default: return type < 0 ? "Unknown" : "Type " + juce::String(type);
    }
}

void RDMManager::requestSlots(int row)
{
    if (row < 0 || row >= devices.size()) return;
    Device& device = devices.getReference(row);
    if (device.slotInfoRequested) return;
    device.slotInfoRequested = true;

    // Probe SLOT_INFO even when it is absent from SUPPORTED_PARAMETERS.
    // Otherwise slotInfoRequested becomes true without queuing any request,
    // leaving the SlotView permanently displaying "Reading slot info...".
    queueRdm(row, 0x0120);

    // Put the user-requested operation ahead of background GETs. RDMManager
    // still processes one request at a time, so this doesn't interrupt an
    // ongoing transaction or an ACK_OVERFLOW continuation.
    for (auto it = requests.begin(); it != requests.end(); ++it)
    {
        if (it->pid == 0x0120 && it->uidText == device.uidText &&
            it->nodeIP == device.nodeIP && it->net == device.net &&
            it->addressLow == device.addressLow)
        {
            Request priorityRequest = std::move(*it);
            requests.erase(it);
            requests.push_front(std::move(priorityRequest));
            break;
        }
    }

    if (!device.supportedPidsRead || device.supportsDefaultSlots)
        queueRdm(row, 0x0122);
}

// Simple live modal, all within RDMManager.cpp: no additional project files.
class RDMManager::SlotView : public juce::Component,
                             private juce::TableListBoxModel,
                             private juce::Timer
{
public:
    SlotView(RDMManager& parent, const Device& target) : owner(&parent),
        uid(target.uidText), ip(target.nodeIP), net(target.net), port(target.addressLow)
    {
        addAndMakeVisible(status);
        status.setJustificationType(juce::Justification::centredLeft);
        addAndMakeVisible(channels);
        channels.setModel(this);
        channels.setRowHeight(25);
        channels.setColour(juce::ListBox::backgroundColourId, backgroundColour);
        channels.getHeader().setColour(juce::TableHeaderComponent::backgroundColourId, headerColour);
        channels.getHeader().addColumn("DMX", 1, 70);
        channels.getHeader().addColumn("Slot type", 2, 112);
        channels.getHeader().addColumn("Function", 3, 240);
        channels.getHeader().addColumn("Default", 4, 90);
        addAndMakeVisible(createButton);
        createButton.setButtonText("Create Fixture Type from RDM");
        juce::Component::SafePointer<SlotView> safe(this);
        createButton.onClick = [safe]()
        {
            if (safe == nullptr || safe->owner == nullptr || safe->created) return;
            int index = safe->owner->findDevice(safe->uid, safe->ip, safe->net, safe->port);
            if (index >= 0)
            {
                safe->owner->createFixtureTypeFromRDM(index);
                safe->created = true;
                safe->timerCallback();
            }
        };
        setSize(590, 540);
        startTimer(200);
        timerCallback();
    }
    ~SlotView() override { stopTimer(); channels.setModel(nullptr); }
    void paint(juce::Graphics& g) override { g.fillAll(backgroundColour); }
    void resized() override
    {
        juce::Rectangle<int> area = getLocalBounds().reduced(7);
        status.setBounds(area.removeFromTop(42));
        createButton.setBounds(area.removeFromBottom(32).removeFromRight(245));
        area.removeFromBottom(5);
        channels.setBounds(area);
    }
private:
    juce::Component::SafePointer<RDMManager> owner;
    juce::String uid, ip;
    int net, port;
    juce::TableListBox channels;
    juce::Label status;
    juce::TextButton createButton;
    int sizeLast = -1;
    bool created = false;
    int rows() const
    {
        if (owner == nullptr) return 0;
        int i = owner->findDevice(uid, ip, net, port);
        if (i < 0) return 0;
        const Device& d = owner->devices.getReference(i);
        return d.footprint > 0 ? juce::jmin(d.footprint, 512) :
            (d.slots.empty() ? 0 : juce::jmin(512, d.slots.rbegin()->first + 1));
    }
    void timerCallback() override
    {
        if (owner == nullptr) return;
        int index = owner->findDevice(uid, ip, net, port);
        if (index < 0) return;
        const Device& d = owner->devices.getReference(index);
        if (sizeLast != rows()) { sizeLast = rows(); channels.updateContent(); }
        // Once SLOT_INFO is available, allow an informed creation. A device
        // without that PID still permits a fallback of unnamed 8-bit channels.
        if (!d.slotInfoRequested && d.personality > 0) owner->requestSlots(index);
        // The same test governs both the button and the status message.
        const bool needsSlots = !d.slotInfoRead && !d.slotInfoUnavailable;
        createButton.setEnabled(!created && d.footprint > 0 && d.footprint <= 512 &&
            !needsSlots);
        createButton.setButtonText(created ? "Fixture Type created" :
            d.slotInfoUnavailable ? "Create basic Fixture Type (no slot map)" :
            "Create Fixture Type from RDM");
        status.setText("Mode " + juce::String(d.personality) + " / " +
            juce::String(d.personalityCount) + "  |  " + juce::String(d.footprint) +
            " DMX channels  |  " + (d.slotInfoRead ? "Slot info received" :
             d.slotInfoUnavailable ? "SLOT_INFO unavailable (basic map only)" :
             d.slotInfoRequested ? "Reading slot info..." : "No slot info"),
            juce::dontSendNotification);
        channels.repaint();
    }
    int getNumRows() override { return rows(); }
    void paintRowBackground(juce::Graphics& g, int, int, int, bool selected) override
    { g.fillAll(selected ? juce::Colour(0xff303c48) : backgroundColour); }
    void paintCell(juce::Graphics& g, int row, int col, int w, int h, bool) override
    {
        if (owner == nullptr) return;
        int index = owner->findDevice(uid, ip, net, port);
        if (index < 0) return;
        const Device& d = owner->devices.getReference(index);
        const auto it = d.slots.find(row);
        juce::String value;
        if (col == 1) value = juce::String(row + 1);
        else if (it != d.slots.end())
        {
            const Slot& slot = it->second;
            if (col == 2) value = slotTypeName(slot.type);
            if (col == 3)
            {
                if (slot.type > 0)
                {
                    value = slotTypeName(slot.type);
                    if (slot.label >= 0 && slot.label < 512)
                        value += " of DMX " + juce::String(slot.label + 1);
                    auto parentSlot = d.slots.find(slot.label);
                    if (parentSlot != d.slots.end())
                        value += " (" + (parentSlot->second.description.isNotEmpty() ?
                            parentSlot->second.description : slotLabelName(parentSlot->second.label)) + ")";
                }
                else value = slot.description.isNotEmpty() ? slot.description : slotLabelName(slot.label);
            }
            if (col == 4 && slot.defaultValue >= 0) value = juce::String(slot.defaultValue);
        }
        if (value.isEmpty()) value = "—";
        g.setColour(juce::Colours::white.withAlpha(0.65f));
        g.drawFittedText(value, 5, 0, w - 8, h, juce::Justification::centredLeft, 1);
    }
};

void RDMManager::showSlots(int row)
{
    if (row < 0 || row >= devices.size()) return;
    requestSlots(row);
    SlotView* view = new SlotView(*this, devices.getReference(row));
    juce::DialogWindow::LaunchOptions options;
    options.dialogTitle = "DMX Channels - " + devices[row].uidText;
    options.dialogBackgroundColour = backgroundColour;
    options.escapeKeyTriggersCloseButton = true;
    options.useNativeTitleBar = false;
    options.resizable = true;
    options.content.setOwned(view);
    options.launchAsync();
}

void RDMManager::createFixtureTypeFromRDM(int row)
{
    if (row < 0 || row >= devices.size()) return;
    // Creating a FixtureType may re-enter UI/manager callbacks.
    // Never hold a reference into devices during that operation.
    const Device device = devices.getReference(row);
    if (device.footprint < 1 || device.footprint > 512) return;
    if (!device.slotInfoRead && !device.slotInfoUnavailable)
        return; // wait for probe result; allow basic map only if unavailable
    // The RDM standard does not define GDTF-style value ranges/functions.
    // Create one BK physical channel per DMX byte; merge only contiguous fine
    // slots explicitly associated with the preceding primary slot.
    FixtureType* ft = FixtureTypeManager::getInstance()->addItem();
    if (ft == nullptr) return;
    juce::String model;
    auto modelIt = device.values.find(0x0080);
    if (modelIt != device.values.end()) model = modelIt->second;
    if (model.isEmpty()) model = device.uidText;
    ft->setNiceName(model + " - RDM mode " + juce::String(device.personality));

    ft->chansManager.massiveImport = true;
    std::set<ChannelType*> usedTypes; // avoid duplicate logical attributes on subfixture 0
    for (int offset = 0; offset < device.footprint; ++offset)
    {
        auto it = device.slots.find(offset);
        const Slot* slot = it == device.slots.end() ? nullptr : &it->second;
        if (slot != nullptr && slot->type == 1 && offset > 0 &&
            slot->label == offset - 1)
        {
            auto previous = device.slots.find(offset - 1);
            if (previous != device.slots.end() && previous->second.type == 0)
                continue; // Fine slot was merged into previous physical channel.
        }
        FixtureTypeChannel* channel = ft->chansManager.addItem();
        if (channel == nullptr) continue;
        bool hasFine = false;
        if (slot != nullptr && slot->type == 0 && offset + 1 < device.footprint)
        {
            auto next = device.slots.find(offset + 1);
            hasFine = next != device.slots.end() && next->second.type == 1 &&
                      next->second.label == offset;
        }
        channel->resolution->setValueWithData(hasFine ? "16bits" :
            slot != nullptr && slot->type == 1 ? "fine" : "8bits");
        // In BK, a single logical channel is represented by channelType on
        // the physical channel. Don't set multiLogicalMode without 2+ attributes.
        if (slot != nullptr && slot->type == 0)
        {
            juce::String label = slotLabelName(slot->label);
            if (label == "Unknown" || label.startsWith("Slot ID 0x"))
                label = slot->description;
            ChannelType* matching = nullptr;
            // Exact match only: ambiguous labels must be mapped by the user.
            for (ChannelFamily* family : ChannelFamilyManager::getInstance()->items)
            {
                for (ChannelType* type : family->definitions.items)
                    if (type->niceName.equalsIgnoreCase(label)) { matching = type; break; }
                if (matching != nullptr) break;
            }
            if (matching != nullptr && usedTypes.insert(matching).second)
                channel->channelType->setValueFromTarget(matching);
            // Repeated logical attributes need user-defined subfixtures.
            // Don't silently overwrite BK's channelsMap for subfixture 0.
        }
        if (slot != nullptr && slot->defaultValue >= 0)
        {
            int initial = slot->defaultValue;
            bool canUseDefault = true;
            if (hasFine)
            {
                auto fine = device.slots.find(offset + 1);
                if (fine != device.slots.end() && fine->second.defaultValue >= 0)
                    initial = initial * 256 + fine->second.defaultValue;
                else canUseDefault = false; // cannot infer a missing fine-byte default
            }
            if (canUseDefault)
                channel->defaultValue->setValue((float)initial / (hasFine ? 65535.0f : 255.0f));
        }
    }
    ft->chansManager.massiveImport = false;
    ft->chansManager.calcDmxChannels();
    statusLabel.setText("Created Fixture Type: " + ft->niceName +
        " (verify channel types and ranges before using)", juce::dontSendNotification);
}

// The action column has its own fixed place at the right of the window.
// Its row coordinates follow the *same* table viewport on vertical scroll.
void RDMManager::ActionsOverlay::paint(juce::Graphics& g)
{
    g.fillAll(backgroundColour);
    const int headerHeight = owner.table.getHeaderHeight();
    g.setColour(headerColour);
    g.fillRect(0, 0, getWidth(), headerHeight);
    g.setColour(juce::Colours::white);
    g.setFont(14.0f);
    g.drawFittedText("...", 0, 0, getWidth(), headerHeight,
                     juce::Justification::centred, 1);
    const juce::Viewport* viewport = owner.table.getViewport();
    if (viewport == nullptr) return;
    const int scrollY = viewport->getViewPositionY();
    const int rowHeight = owner.table.getRowHeight();
    const int visibleBottom = headerHeight + viewport->getHeight();
    const int first = juce::jmax(0, scrollY / rowHeight);
    const int last = juce::jmin(owner.devices.size(),
        (scrollY + getHeight() + rowHeight - 1) / rowHeight);
    for (int row = first; row < last; ++row)
    {
        const int y = headerHeight + row * rowHeight - scrollY;
        if (y + rowHeight <= headerHeight || y >= visibleBottom) continue;
        g.setColour(backgroundColour);
        g.fillRect(0, y, getWidth(), rowHeight);
        g.setColour(juce::Colours::white);
        g.setFont(16.0f);
        g.drawFittedText("...", 0, y, getWidth(), rowHeight,
                         juce::Justification::centred, 1);
    }
}

void RDMManager::ActionsOverlay::mouseUp(const juce::MouseEvent& e)
{
    const int y = e.y - owner.table.getHeaderHeight();
    if (y < 0) return;
    const juce::Viewport* vp = owner.table.getViewport();
    if (vp == nullptr || y >= vp->getHeight()) return;
    const int row = (y + vp->getViewPositionY()) / owner.table.getRowHeight();
    if (row >= 0 && row < owner.devices.size())
        owner.showDeviceMenu(row);
}

void RDMManager::ActionsOverlay::mouseWheelMove(const juce::MouseEvent&,
                                               const juce::MouseWheelDetails& wheel)
{
    auto& bar = owner.table.getVerticalScrollBar();
    bar.setCurrentRangeStart(bar.getCurrentRangeStart() - wheel.deltaY *
                             (owner.table.getRowHeight() * 3));
    repaint();
}
