/*
  ==============================================================================

	DMXArtNetDevice.cpp
	Created: 10 Apr 2017 12:44:42pm
	Author:  Ben

  ==============================================================================
*/

#include "../../CommonIncludes.h"
#include "Definitions/Interface/InterfaceIncludes.h"

juce_ImplementSingleton(ArtnetSocket);

ArtnetSocket::ArtnetSocket() :
	Thread("BKArtnetPort")
{
	memset(receiveBuffer, 0, MAX_PACKET_LENGTH);
	memset(artnetPacket + DMX_HEADER_LENGTH, 0, NUM_CHANNELS);
	socket = std::make_unique <DatagramSocket>(true);
	socket->bindToPort(0);
	startThread();
}

ArtnetSocket :: ~ArtnetSocket() {
	if (socket != nullptr) {
		socket->shutdown();
	}
	signalThreadShouldExit();
	waitForThreadToExit(100);
	clearSingletonInstance();
}

void ArtnetSocket::selectSocketPort()
{
	bool forceBind = false;
	for (auto& i : InterfaceManager::getInstance()->items)
	{
		DMXInterface* inter = dynamic_cast<DMXInterface*>(i);
		if (inter == nullptr) continue;
		DMXArtNetDevice* dev = dynamic_cast<DMXArtNetDevice*>(inter->dmxDevice.get());
		if (dev== nullptr) continue;
		if (dev->forceSrcPort->boolValue()) {
			forceBind = true;
		}
		if (dev->inputCC->enabled->boolValue()) {
			forceBind = true;
		}
	}

	if (socket != nullptr) socket->shutdown();

	socket.reset(new DatagramSocket(true));

	if (forceBind) {
		if (socket->bindToPort(6454)) {
			LOG("bind to port 6454");
		}
		else {
			LOGERROR("Error binding port 6454, is it already taken ?");
		}
	}
	else {
		socket->bindToPort(0);
		LOG("port 6454 is now free");
	}
}

void ArtnetSocket::sendArtPoll(String ip)
{
	isSending.enter();
	socket->write(ip, 6454, artPollPacket, 18);
	isSending.exit();
}

void ArtnetSocket::sendArtPollReply(String ip)
{
	artPollReplyPacket[14] = 0X36; //port low
	artPollReplyPacket[15] = 0X19; // port high
	artPollReplyPacket[16] = 0; // version H
	artPollReplyPacket[17] = 1; // version L
	artPollReplyPacket[18] = 0; // netSwitch
	artPollReplyPacket[19] = 0; // subSwitch
	artPollReplyPacket[20] = 0x12; // OEM Hi
	artPollReplyPacket[21] = 0x12; // OEM
	artPollReplyPacket[22] = 0; // ubea
	artPollReplyPacket[23] = 0b11010000; // status 1
	artPollReplyPacket[24] = 0; // EstaMan lo
	artPollReplyPacket[25] = 0; // EstaMan HI
	artPollReplyPacket[26] = 0; // shortname (18)
	String shortname = "Blinderkitten";
	for (int i = 0; i < shortname.length(); i++) {
		artPollReplyPacket[26 + i] = shortname[i];
	}
	String longname = "Blinderkitten";
	for (int i = 0; i < longname.length(); i++) {
		artPollReplyPacket[44 + i] = longname[i];
	}
	String status = "Everything is purrfect";
	for (int i = 0; i < status.length(); i++) {
		artPollReplyPacket[108 + i] = status[i];
	}
	artPollReplyPacket[172] = 0; // NumPortsHi
	artPollReplyPacket[173] = 0; // NumPortsLow
	artPollReplyPacket[174] = 0; // PortTypes1
	artPollReplyPacket[174] = 0; // PortTypes2
	artPollReplyPacket[174] = 0; // PortTypes3
	artPollReplyPacket[174] = 0; // PortTypes4

	StringArray receiveAdress = StringArray::fromTokens(ip, ".", "");

	auto ips = IPAddress::getAllAddresses(false);
	for (int i = 0; i < ips.size(); i++) {
		String ip2 = ips[i].toString();
		StringArray sendAddress = StringArray::fromTokens(ip2, ".", "");
		if (sendAddress[0] == receiveAdress[0]) {
			artPollReplyPacket[10] = ips[i].address[0];
			artPollReplyPacket[11] = ips[i].address[1];
			artPollReplyPacket[12] = ips[i].address[2];
			artPollReplyPacket[13] = ips[i].address[3];
			DatagramSocket tempSender = DatagramSocket(true);
			tempSender.bindToPort(0, ip2);
			tempSender.write("255.255.255.255", 6454, artPollReplyPacket, 287);
		}
	}


}


DMXArtNetDevice::DMXArtNetDevice() :
	DMXDevice("ArtNet", ARTNET, true)
{

//	localPort = inputCC->addIntParameter("Local Port", "Local port to receive ArtNet data. This needs to be enabled in order to receive data", 6454, 0, 65535);
	inputNet = inputCC->addIntParameter("Net", "The net to receive from, from 0 to 15", 0, 0, 127);
	inputSubnet = inputCC->addIntParameter("Subnet", "The subnet to receive from, from 0 to 15", 0, 0, 15);
	inputUniverse = inputCC->addIntParameter("Universe", "The Universe to receive from, from 0 to 15", 0, 0, 15);
	inputCC->enabled->setValue(false);

	remoteHost = outputCC->addStringParameter("Remote Host", "IP to which send the Art-Net to", "127.0.0.1");
	remotePort = outputCC->addIntParameter("Remote Port", "Remote port to send ArtNet data", 6454, 0, 65535);
	outputNet = outputCC->addIntParameter("Net", "The net to send to, from 0 to 15", 0, 0, 127);
	outputSubnet = outputCC->addIntParameter("Subnet", "The subnet to send to, from 0 to 15", 0, 0, 15);
	outputUniverse = outputCC->addIntParameter("Universe", "The Universe to send to, from 0 to 15", 0, 0, 15);
	forceSrcPort = outputCC->addBoolParameter("Force src port", "the source port is forced to 6454, dissable this port for other applications, but some artnet devices work only if source port equals dest port.", false);

	memset(artnetPacket + DMX_HEADER_LENGTH, 0, NUM_CHANNELS);

	discoverNodesIP = addStringParameter("Discover IP", "Find nodes on this IP, Input must be enabled to receive responses !", "2.255.255.255");
	findNodesBtn = addTrigger("Find nodes", "find artnet nodes");

	sendDataAtStartup();
	ArtnetSocket::getInstance()->selectSocketPort();
}

DMXArtNetDevice::~DMXArtNetDevice()
{
	if (Engine::mainEngine != nullptr) Engine::mainEngine->removeEngineListener(this);
}

void DMXArtNetDevice::triggerTriggered(Trigger* t)
{
	if (t == findNodesBtn) { sendArtPoll(); }
}

void DMXArtNetDevice::setupReceiver()
{
	ArtnetSocket::getInstance()->selectSocketPort();
	/*
	stopThread(500);
	setConnected(false);
	if (receiver != nullptr) receiver->shutdown();

	if (!inputCC->enabled->boolValue())
	{
		clearWarning();
		return;
	}

	receiver.reset(new DatagramSocket());
	bool result = receiver->bindToPort(6454);
	if (result)
	{
		receiver->setEnablePortReuse(false);
		clearWarning();
		NLOG(niceName, "Listening for ArtNet on port 6454");
	}
	else
	{
		setWarningMessage("Error binding port 6454, is it already taken ?");
		return;
	}

	startThread();
	setConnected(true);
	*/
}

void DMXArtNetDevice::sendDMXValue(int channel, int value)
{
	artnetPacket[channel-1 + DMX_HEADER_LENGTH] = (uint8)value;
	DMXDevice::sendDMXValue(channel, value);
}

void DMXArtNetDevice::sendDMXRange(int startChannel, Array<int> values)
{
	int numValues = values.size();
	for (int i = 0; i < numValues; ++i)
	{
		int channel = startChannel + i;
		if (channel < 0) continue;
		if (channel > 512) break;

		artnetPacket[channel - 1 + DMX_HEADER_LENGTH] = (uint8)(values[i]);
	}
	

	DMXDevice::sendDMXRange(startChannel, values);

}

void DMXArtNetDevice::sendDMXValuesInternal()
{
	if (outputUniverse == nullptr) return;
	sequenceNumber = (sequenceNumber + 1) % 256;

	artnetPacket[12] = sequenceNumber;
	artnetPacket[13] = 0;
	artnetPacket[14] = (outputSubnet->intValue() << 4) | outputUniverse->intValue();
	artnetPacket[15] = outputNet->intValue();
	artnetPacket[16] = 2;
	artnetPacket[17] = 0;
	int test = (outputSubnet->intValue() << 4) | outputUniverse->intValue();

	ArtnetSocket::getInstance()->isSending.enter();
	ArtnetSocket::getInstance()->socket->write(remoteHost->stringValue(), remotePort->intValue(), artnetPacket, 530);
	ArtnetSocket::getInstance()->isSending.exit();
}

void DMXArtNetDevice::sendArtPoll()
{
	ArtnetSocket::getInstance()->sendArtPoll(discoverNodesIP->stringValue());
}

void DMXArtNetDevice::endLoadFile()
{
	Engine::mainEngine->removeEngineListener(this);
	setupReceiver();
}

void DMXArtNetDevice::onControllableFeedbackUpdate(ControllableContainer* cc, Controllable* c)
{
	DMXDevice::onControllableFeedbackUpdate(cc, c);
	if (c == inputCC->enabled || c == forceSrcPort) setupReceiver();
	sendDataAtStartup();
}

void ArtnetSocket::run()
{
	while (!threadShouldExit())
	{
		if (socket == nullptr) {
			sleep(10); //100fps
		}
		else {
			String rAddress = "";
			int rPort;
			int bytesRead = socket->read(receiveBuffer, MAX_PACKET_LENGTH, false, rAddress, rPort);
			if (bytesRead > 0)
			{
				for (uint8 i = 0; i < 8; ++i)
				{
					if (receiveBuffer[i] != artnetPacket[i])
					{
						NLOGWARNING("Art-net", "Received packet is not valid ArtNet");
						break;
					}
				}

				int opcode = receiveBuffer[8] | receiveBuffer[9] << 8;

				if (opcode == DMX_OPCODE)
				{
					//int sequence = artnetPacket[12];
					int universe = receiveBuffer[14] & 0xF;
					int subnet = (receiveBuffer[14] >> 4) & 0xF;
					int net = receiveBuffer[15] & 0x7F;

					for (auto& i : InterfaceManager::getInstance()->items)
					{
						DMXInterface* inter = dynamic_cast<DMXInterface*>(i);
						if (inter == nullptr) continue;
						DMXArtNetDevice* dev = dynamic_cast<DMXArtNetDevice*>(inter->dmxDevice.get());
						if (dev == nullptr) continue;
						if (net == dev->inputNet->intValue() && subnet == dev->inputSubnet->intValue() && universe == dev->inputUniverse->intValue())
						{
							int dmxDataLength = jmin(receiveBuffer[17] | receiveBuffer[16] << 8, NUM_CHANNELS);
							dev->setDMXValuesIn(dmxDataLength, receiveBuffer + DMX_HEADER_LENGTH);
						}
					}
				}
				else if (opcode == OPPOLL)
				{
					//DBG("Received ArtPoll");
					sendArtPollReply(rAddress);
				}
				else if (opcode == POLLRESPONSE_OPCODE)
				{
					String ip = "";
					ip += String(receiveBuffer[10]) + ".";
					ip += String(receiveBuffer[11]) + ".";
					ip += String(receiveBuffer[12]) + ".";
					ip += String(receiveBuffer[13]);

					String shortName = "";
					bool continueName = true;
					for (int i = 0; i < 18 && continueName; i++) {
						char c = char(receiveBuffer[i + 26]);
						if ((int)c == 0) {continueName = false;}
						else { shortName += c; }
					}
					LOG("node replied ! "+ip+ " : "+shortName);
				}
				else
				{
					LOG("ArtNet OpCode not handled : " << opcode << "( 0x" << String::toHexString(opcode) << ")");
				}
			}
		else
			{
			sleep(10); //100fps
			}
		}
	}
}

void DMXArtNetDevice::sendDataAtStartup()
{
	DMXDevice::sendDMXValue(1, dmxDataOut[0]);
}

