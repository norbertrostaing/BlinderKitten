#include <algorithm>
#include <cstring>
#include "RDMManager.h"
#include "Definitions/Interface/InterfaceIncludes.h"
#include "Definitions/Fixture/FixtureManager.h"
#include "Definitions/Fixture/FixturePatch.h"
#include "Definitions/Fixture/Fixture.h"

namespace
{
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
        case 0x00e0: return "Personality";
        case 0x00e1: return "Personality list";
        case 0x00f0: return "DMX address";
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
        case 0x1000: return "Identify";
        default: return "PID 0x" + juce::String::toHexString(pid).paddedLeft('0', 4).toUpperCase();
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
        case 0x0500: case 0x0501: case 0x0600: case 0x0601: case 0x1000:
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
    table.setMultipleSelectionEnabled(false);
    table.setColour(juce::ListBox::backgroundColourId, juce::Colour(29, 31, 35));
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

    columns = { 0x0081, 0x0080, 0x0082, 0x00f0, 0x00e0, 0x0400, 0x0401, 0x00c0, 0x0060, 0x0050 };
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

void RDMManager::resized()
{
    juce::Rectangle<int> area = getLocalBounds().reduced(4);
    juce::Rectangle<int> top = area.removeFromTop(28);
    scanButton.setBounds(top.removeFromLeft(140).reduced(2));
    statusLabel.setBounds(top);
    area.removeFromTop(4);
    table.setBounds(area);
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
    columns = { 0x0081, 0x0080, 0x0082, 0x00f0, 0x00e0, 0x0400, 0x0401, 0x00c0, 0x0060, 0x0050 };
    rebuildColumns();
    statusLabel.setText("Ready", juce::dontSendNotification);
    table.updateContent();
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
            int index = findDevice(activeRequest.uidText, activeRequest.nodeIP,
                activeRequest.net, activeRequest.addressLow);
            if (index >= 0 && !activeRequest.set)
                devices.getReference(index).values[activeRequest.pid] = "Timeout";
            hasActiveRequest = false;
            table.repaint();
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
        devices.add(device);
        int index = devices.size() - 1;
        const int initialPids[] = { 0x0050, 0x0060, 0x0080, 0x0081, 0x0082, 0x00c0, 0x00e0, 0x00f0, 0x0400, 0x0401 };
        for (int pid : initialPids) queueRdm(index, pid);
    }
    table.updateContent();
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
    hasActiveRequest = false;

    if (rdm[15] == 0x01) // ACK_TIMER
    {
        if (!wasSet) device.values[pid] = "ACK_TIMER";
        table.repaint();
        return;
    }
    if (rdm[15] == 0x02) // NACK_REASON
    {
        if (!wasSet) device.values[pid] = "NACK 0x" + juce::String::toHexString(pdl >= 2 ? read16(rdm + 23) : 0);
        table.repaint();
        return;
    }
    if (rdm[15] != 0x00) return;

    const uint8* data = rdm + 23;
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
        for (int i = 0; i + 1 < pdl; i += 2)
        {
            int supportedPid = read16(data + i);
            addColumn(supportedPid);
            if (supportedPid == 0x0050 || supportedPid == 0x0051 || supportedPid == 0x0200 ||
                supportedPid == 0x0201 || supportedPid == 0x00e1 || supportedPid == 0x0121 ||
                supportedPid == 0x0120 || supportedPid == 0x0031) continue; // needs parameter or specialized decoding
            if (supportedPid >= 0x0010 && supportedPid != 0x1001 && supportedPid != 0x0090 &&
                device.requested.size() < 160)
                queueRdm(index, supportedPid);
        }
        device.values[pid] = juce::String(pdl / 2) + " PIDs";
    }
    else
    {
        if (pid == 0x0201 && pdl >= 3)
        {
            if (device.values[pid].isNotEmpty()) device.values[pid] += "; ";
            device.values[pid] += decodeValue(pid, data, pdl);
        }
        else if (pid == 0x00e1 && pdl >= 3)
        {
            if (device.values[pid].isNotEmpty()) device.values[pid] += "; ";
            device.values[pid] += juce::String(data[0]) + ": " + juce::String::fromUTF8((const char*)(data + 3), pdl - 3);
        }
        else device.values[pid] = decodeValue(pid, data, pdl);
    }
    if (pid == 0x00f0 && pdl >= 2) device.address = read16(data);
    if (pid == 0x00e0 && pdl >= 2)
    {
        device.personality = data[0];
        device.personalityCount = data[1];
        for (int mode = 1; mode <= juce::jmin(device.personalityCount, 16); ++mode)
        {
            uint8 personality = uint8(mode);
            juce::MemoryBlock payload(&personality, 1);
            queueRdm(index, 0x00e1, false, payload);
        }
    }
    if (pid == 0x0060 && pdl >= 19)
    {
        device.personality = data[12];
        device.personalityCount = data[13];
        device.address = read16(data + 14);
        for (int sensor = 0; sensor < juce::jmin((int)data[18], 16); ++sensor)
        {
            uint8 sensorNumber = uint8(sensor);
            juce::MemoryBlock payload(&sensorNumber, 1);
            queueRdm(index, 0x0201, false, payload);
        }
    }
    addColumn(pid);
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
    header.addColumn("Fixture BK", 2, 145, 80, 300);
    header.addColumn("Interface", 5, 145, 90, 260);
    header.addColumn("Universe", 6, 105, 90, 150);
    header.addColumn("Node IP", 3, 120, 80, 190);
    for (int pid : columns)
        header.addColumn(pidName(pid), pid + 1000, 150, 60, 400);
    header.addColumn("...", 4, 44, 44, 44);
    table.updateContent();
}

int RDMManager::getNumRows() { return devices.size(); }

juce::String RDMManager::getFixtureName(const Device& device) const
{
    if (device.assignedFixture.isNotEmpty()) return device.assignedFixture;
    if (device.output == nullptr || device.address == 0) return "—";
    for (Fixture* fixture : FixtureManager::getInstance()->items)
    {
        for (FixturePatch* patch : fixture->patchs.items)
        {
            if (patch->targetInterface->targetContainer.get() == device.output &&
                patch->address->intValue() == device.address)
                return fixture->id->stringValue() + " " + fixture->userName->stringValue();
        }
    }
    return "—";
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
    if (columnId == 4) return "...";
    int pid = columnId - 1000;
    auto it = device.values.find(pid);
    return it == device.values.end() ? juce::String() : it->second;
}

void RDMManager::paintRowBackground(juce::Graphics& g, int row, int width, int height, bool selected)
{
    g.fillAll(selected ? juce::Colour(56, 75, 92) : ((row & 1) ? juce::Colour(37, 39, 44) : juce::Colour(29, 31, 35)));
}

void RDMManager::paintCell(juce::Graphics& g, int row, int columnId, int width, int height, bool)
{
    g.setColour(juce::Colours::white.withAlpha(0.9f));
    g.setFont(12.0f);
    g.drawFittedText(cellText(row, columnId), 5, 0, width - 9, height, juce::Justification::centredLeft, 1);
    g.setColour(juce::Colours::white.withAlpha(0.09f));
    g.drawVerticalLine(width - 1, 0.0f, (float)height);
}

void RDMManager::cellClicked(int row, int columnId, const juce::MouseEvent&)
{
    if (columnId == 4) showDeviceMenu(row);
}

void RDMManager::cellDoubleClicked(int row, int columnId, const juce::MouseEvent&)
{
    if (columnId != 4) editCell(row, columnId);
}

void RDMManager::editCell(int row, int columnId)
{
    if (row < 0 || row >= devices.size()) return;
    int pid = columnId - 1000;
    if (pid != 0x0082 && pid != 0x00f0 && pid != 0x00e0 && pid != 0x0500 && pid != 0x0501 &&
        pid != 0x0600 && pid != 0x0601) return;
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
                if (pid == 0x00e0 && (number < 1 || number > safe->devices[row].personalityCount)) return;
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
    juce::PopupMenu fixtures;
    int n = 0;
    for (Fixture* fixture : FixtureManager::getInstance()->items)
        fixtures.addItem(1000 + n++, fixture->id->stringValue() + " - " + fixture->userName->stringValue());
    menu.addSubMenu("Use for BK fixture", fixtures, n > 0 && devices[row].address > 0);
    juce::Component::SafePointer<RDMManager> safe(this);
    menu.showMenuAsync(juce::PopupMenu::Options(), [safe, row](int result)
    {
        if (safe == nullptr || row >= safe->devices.size() || result == 0) return;
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
        else if (result >= 1000) safe->associateFixture(row, result - 1000);
    });
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
    devices.getReference(row).assignedFixture = fixture->id->stringValue() + " " + fixture->userName->stringValue();
    table.repaint();
}
