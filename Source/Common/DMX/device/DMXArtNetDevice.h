/*
  ==============================================================================

    DMXArtNetDevice.h
    Created: 10 Apr 2017 12:44:42pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#define DMX_OPCODE 0x5000
#define OPPOLL 0x2000
#define POLLRESPONSE_OPCODE 0x2100
#define OPTIMECODE 0X9700
#define PROTOCOL_VERSION 14
#define NUM_CHANNELS 512
#define DMX_HEADER_LENGTH 18
#define MAX_PACKET_LENGTH DMX_HEADER_LENGTH+NUM_CHANNELS

class ArtnetSocket :
	public Thread 
{
	public:
	juce_DeclareSingleton(ArtnetSocket, true);
	ArtnetSocket();
	~ArtnetSocket();

	uint8 artnetPacket[MAX_PACKET_LENGTH]{ 'A','r','t','-','N','e','t',0, 0x00 , 0x50,  0, PROTOCOL_VERSION };
	uint8 artPollPacket[MAX_PACKET_LENGTH]{ 'A','r','t','-','N','e','t',0, 0x00 , 0x20,  0, PROTOCOL_VERSION };
	uint8 artPollReplyPacket[287]{ 'A','r','t','-','N','e','t',0, 0x00 , 0x21 };
	uint8 receiveBuffer[MAX_PACKET_LENGTH];

	void run() override;
	std::unique_ptr<DatagramSocket> socket;
	uint8 sequenceNumber;
	void selectSocketPort();
	void sendArtPoll(String ip);
	void sendArtPollReply(String ip);
	CriticalSection isSending;

};

class DMXArtNetDevice :
	public DMXDevice,
	public EngineListener{
public:
	DMXArtNetDevice();
	~DMXArtNetDevice();

	//EnumParameter * networkInterface;

//	IntParameter* localPort;
	IntParameter* inputNet;
	IntParameter* inputSubnet;
	IntParameter* inputUniverse;
	//StringParameter * nodeName;

	StringParameter* remoteHost;
	IntParameter * remotePort;
	IntParameter* outputNet;
	IntParameter* outputSubnet;
	IntParameter* outputUniverse = nullptr;
	BoolParameter* forceSrcPort;
	BoolParameter* shouldSendArtSync;

	StringParameter* discoverNodesIP;
	Trigger* findNodesBtn;

	uint8 artnetPacket[MAX_PACKET_LENGTH]{ 'A','r','t','-','N','e','t',0, 0x00 , 0x50,  0, PROTOCOL_VERSION };
	uint8 artnetSync[14]{ 'A','r','t','-','N','e','t',0, 0x00 , 0x52,  0, PROTOCOL_VERSION, 0, 0 };
	uint8 sequenceNumber;

	void triggerTriggered(Trigger* t) override;

	void setupReceiver();

	void sendDMXValue(int channel, int value) override;
	void sendDMXRange(int startChannel, Array<int> values) override;

	void sendDMXValuesInternal() override;
	void sendArtPoll();
	void sendArtSync();

	void endLoadFile() override;

	void onControllableFeedbackUpdate(ControllableContainer * cc, Controllable * c) override;
	
	void sendDataAtStartup();
};

