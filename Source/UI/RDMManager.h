#pragma once

#include <JuceHeader.h>
#include <array>
#include <deque>
#include <map>
#include <set>
#include <vector>

class DMXInterface;
class DMXArtNetDevice;

// Dockable RDM window. The transport uses the existing ArtnetSocket.
class RDMManager : public ShapeShifterContentComponent,
                   private juce::TableListBoxModel,
                   private juce::Timer
{
public:
    RDMManager();
    ~RDMManager() override;

    static RDMManager* create(const juce::String&) { return new RDMManager(); }

    // Call from ArtnetSocket::run(), on its receiving thread.
    static void receiveArtNet(const uint8* bytes, int length, const juce::String& senderIP);

    void resized() override;

private:
    struct Device
    {
        std::array<uint8, 6> uid{};
        juce::String uidText, nodeIP, assignedFixture;
        DMXInterface* output = nullptr;
        int net = 0;
        int addressLow = 0;
        std::map<int, juce::String> values;
        std::set<int> requested;
        int address = 0;
        int personality = 0;
        int personalityCount = 0;
        bool identify = false;
    };

    struct Packet
    {
        juce::MemoryBlock data;
        juce::String senderIP;
    };

    struct Request
    {
        juce::String uidText, nodeIP;
        DMXInterface* output = nullptr;
        int net = 0;
        int addressLow = 0;
        std::array<uint8, 6> uid{};
        int pid = 0;
        bool set = false;
        juce::MemoryBlock payload;
        uint8 transaction = 0;
        int retries = 0;
        double sentAt = 0;
    };

    juce::TextButton scanButton { "Scan all RDM" };
    juce::Label statusLabel;
    juce::TableListBox table;
    juce::Array<DMXInterface*> outputs;
    juce::Array<Device> devices;
    std::vector<int> columns;

    juce::CriticalSection receiveLock;
    std::deque<Packet> receivedPackets;
    std::deque<Request> requests;
    Request activeRequest;
    bool hasActiveRequest = false;
    bool scanActive = false;
    bool scanEverStarted = false;
    double scanStarted = 0;
    double lastTodRequest = 0;
    uint8 transactionNumber = 0;
    std::array<uint8, 6> controllerUID{};

    void refreshOutputs();
    void resetScan();
    void startScan();
    bool sendArtNet(DMXInterface* output, const juce::String& ip, const uint8* data, int size);
    void sendTodControl(DMXInterface* output);
    void sendTodRequest(DMXInterface* output);
    void sendRdm(Request& request);
    void queueRdm(int deviceIndex, int pid, bool set = false, const juce::MemoryBlock& payload = {});
    void timerCallback() override;
    void processPacket(const Packet& packet);
    void processTod(const uint8* bytes, int size, const juce::String& senderIP);
    void processRdm(const uint8* bytes, int size, const juce::String& senderIP);
    int findDevice(const juce::String& uid, const juce::String& ip, int net, int addressLow) const;
    void addColumn(int pid);
    void rebuildColumns();
    void showDeviceMenu(int row);
    void editCell(int row, int columnId);
    void associateFixture(int row, int fixtureIndex);
    juce::String getFixtureName(const Device& device) const;
    juce::String cellText(int row, int columnId) const;

    int getNumRows() override;
    void paintRowBackground(juce::Graphics&, int, int, int, bool) override;
    void paintCell(juce::Graphics&, int, int, int, int, bool) override;
    void cellClicked(int, int, const juce::MouseEvent&) override;
    void cellDoubleClicked(int, int, const juce::MouseEvent&) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RDMManager)
};
