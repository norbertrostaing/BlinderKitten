#pragma once

#include <JuceHeader.h>
#include <array>
#include <deque>
#include <map>
#include <set>
#include <string>
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

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    struct Slot
    {
        int offset = 0;  // RDM offset: zero based
        int type = -1;   // RDM Slot Type: 0 primary, 1 fine, ...
        int label = 0xffff; // RDM Slot Label ID (or primary offset for secondary slots)
        int defaultValue = -1;
        juce::String description;
    };

    struct Device
    {
        std::array<uint8, 6> uid{};
        juce::String uidText, nodeIP;
        int associatedFixtureId = 0;
        std::map<int, juce::String> personalityDescriptions;
        std::map<int, Slot> slots;
        int footprint = 0;
        int modelId = -1;
        bool deviceInfoRead = false;
        bool supportedPidsRead = false;
        bool supportsSlotInfo = false, supportsSlotDescription = false, supportsDefaultSlots = false;
        bool slotInfoRequested = false, slotInfoRead = false, slotInfoUnavailable = false;
        DMXInterface* output = nullptr; // optional BK patch interface; RDM is routed by node IP and universe
        int udpPort = 6454;
        int net = 0;
        int addressLow = 0;
        std::map<int, juce::String> values;
        std::set<int> requested;
        int address = 0;
        int personality = 0;
        int personalityCount = 0;
        bool identify = false;
    };

    struct DiscoveredPort
    {
        juce::String nodeIP, nodeName;
        DMXInterface* output = nullptr;  // matched to a BK output if it already exists
        int net = 0;
        int addressLow = 0;
        int udpPort = 6454;
        bool supportsRDM = false;        // advertised by ArtPollReply.Status1 bit 1
        bool flushSent = false;
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
        int udpPort = 6454;
        int net = 0;
        int addressLow = 0;
        std::array<uint8, 6> uid{};
        int pid = 0;
        bool set = false;
        juce::MemoryBlock payload;
        uint8 transaction = 0;
        int retries = 0;
        juce::MemoryBlock overflowData;
        int overflowCount = 0;
        double sentAt = 0;
    };

    juce::TextButton scanButton { "Scan all RDM" };
    juce::TextButton createDummyButton { "Create dummy fixtures" };
    juce::Label statusLabel;
    juce::TableListBox table;

    // Fixed action gutter, independent of the horizontally scrollable table.
    class ActionsOverlay : public juce::Component
    {
    public:
        explicit ActionsOverlay(RDMManager& owner) : owner(owner) {}
        void paint(juce::Graphics& g) override;
        void mouseUp(const juce::MouseEvent& e) override;
        void mouseWheelMove(const juce::MouseEvent& e,
                            const juce::MouseWheelDetails& wheel) override;
    private:
        RDMManager& owner;
    };
    ActionsOverlay actionsOverlay { *this };
    int lastTableScrollY = -1;
    juce::Array<DMXInterface*> outputs;
    DMXInterface* forcedSourceOutput = nullptr;
    std::vector<DiscoveredPort> discoveredPorts;
    juce::Array<Device> devices;
    std::vector<int> columns;
    std::map<std::string, int> associations; // Explicit UID / node / port -> fixture ID, kept across rescans

    juce::CriticalSection receiveLock;
    std::deque<Packet> receivedPackets;
    std::deque<Request> requests;
    Request activeRequest;
    bool hasActiveRequest = false;
    bool scanActive = false;
    bool scanEverStarted = false;
    double scanStarted = 0;
    double lastTodRequest = 0;
    double lastArtPoll = 0;
    uint8 transactionNumber = 0;
    std::array<uint8, 6> controllerUID{};

    void refreshOutputs();
    DMXInterface* findConfiguredOutput(const juce::String& nodeIP, int net, int addressLow) const;
    DMXInterface* ensureInterface(int row);
    void addDiscoveredPort(const juce::String& ip, int net, int addressLow,
                           bool supportsRDM, const juce::String& name = {});
    void resetScan();
    void restoreSourcePort();
    void startScan();
    bool sendArtNet(const juce::String& ip, const uint8* data, int size, int udpPort = 6454);
    void sendArtPoll();
    void sendTodControl(const DiscoveredPort& target);
    void sendTodRequest(const DiscoveredPort& target);
    void sendRdm(Request& request);
    void queueRdm(int deviceIndex, int pid, bool set = false, const juce::MemoryBlock& payload = {});
    void timerCallback() override;
    void processPacket(const Packet& packet);
    void processPollReply(const uint8* bytes, int size, const juce::String& senderIP);
    void processTod(const uint8* bytes, int size, const juce::String& senderIP);
    void processRdm(const uint8* bytes, int size, const juce::String& senderIP);
    int findDevice(const juce::String& uid, const juce::String& ip, int net, int addressLow) const;
    std::string deviceKey(const Device& device) const;
    void addColumn(int pid);
    void rebuildColumns();
    void showDeviceMenu(int row);
    void editCell(int row, int columnId);
    void associateFixture(int row, int fixtureIndex);
    void createFixture(int row, int fixtureTypeIndex);
    void createFixtureTypeFromRDM(int row);
    void createDummyFixtures(bool confirmed = false);
    void requestSlots(int row);
    void showSlots(int row);
    static juce::String slotLabelName(int id);
    static juce::String slotTypeName(int type);
    class SlotView;
    juce::String getFixtureName(const Device& device) const;
    juce::String cellText(int row, int columnId) const;

    int getNumRows() override;
    void paintRowBackground(juce::Graphics&, int, int, int, bool) override;
    void paintCell(juce::Graphics&, int, int, int, int, bool) override;
    juce::Component* refreshComponentForCell(int, int, bool, juce::Component*) override;
    void cellClicked(int, int, const juce::MouseEvent&) override;
    void cellDoubleClicked(int, int, const juce::MouseEvent&) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RDMManager)
};
