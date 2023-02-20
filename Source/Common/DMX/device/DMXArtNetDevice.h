/*
  ==============================================================================

    DMXArtNetDevice.h
    Created: 10 Apr 2017 12:44:42pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#define DMX_OPCODE 0x5000
#define POLLRESPONSE_OPCODE 0x2100
#define PROTOCOL_VERSION 14
#define NUM_CHANNELS 512
#define DMX_HEADER_LENGTH 18
#define MAX_PACKET_LENGTH DMX_HEADER_LENGTH+NUM_CHANNELS

class DMXArtNetDevice :
	public DMXDevice,
	public EngineListener,
	public Thread //receiving
{
public:
	DMXArtNetDevice();
	~DMXArtNetDevice();

	//EnumParameter * networkInterface;

	IntParameter* localPort;
	IntParameter* inputNet;
	IntParameter* inputSubnet;
	IntParameter* inputUniverse;
	//StringParameter * nodeName;

	StringParameter* remoteHost;
	IntParameter * remotePort;
	IntParameter* outputNet;
	IntParameter* outputSubnet;
	IntParameter* outputUniverse;

	StringParameter* discoverNodesIP;
	Trigger* findNodesBtn;

	std::unique_ptr<DatagramSocket> receiver;
	DatagramSocket sender;

	uint8 sequenceNumber;
	uint8 artnetPacket[MAX_PACKET_LENGTH]{ 'A','r','t','-','N','e','t',0, 0x00 , 0x50,  0, PROTOCOL_VERSION };
	uint8 artPollPacket[MAX_PACKET_LENGTH]{ 'A','r','t','-','N','e','t',0, 0x00 , 0x20,  0, PROTOCOL_VERSION };
	uint8 receiveBuffer[MAX_PACKET_LENGTH];

	void triggerTriggered(Trigger* t);

	void setupReceiver();

	void sendDMXValue(int channel, int value) override;
	void sendDMXRange(int startChannel, Array<int> values) override;

	void sendDMXValuesInternal() override;
	void sendArtPoll();

	void endLoadFile() override;

	void onControllableFeedbackUpdate(ControllableContainer * cc, Controllable * c) override;
	
	void run() override;

	void sendDataAtStartup();
};

