
#if JUCE_LINUX || JUCE_MAC
#include <sys/ioctl.h>
#if JUCE_LINUX
#include <asm/termbits.h>
#elif JUCE_MAC
#include <IOKit/serial/ioss.h>
#endif
#endif

DMXEuroliteDevice::DMXEuroliteDevice() :
	DMXSerialDevice("Eurolite", EUROLITE, false)
{
}

DMXEuroliteDevice::~DMXEuroliteDevice()
{
}

void DMXEuroliteDevice::setPortConfig()
{
	try
	{
		dmxPort->port->setBaudrate(250000);
		dmxPort->port->setBytesize(serial::eightbits);
		dmxPort->port->setStopbits(serial::stopbits_two);
		dmxPort->port->setParity(serial::parity_none);
		dmxPort->port->setFlowcontrol(serial::flowcontrol_none);
		dmxPort->port->setRTS(false);
		dmxPort->port->setDTR(false);
		dmxPort->port->flush();
	}
	catch (serial::IOException e)
	{
		LOGERROR("Error opening Eurolite device: " << e.what());
	}

}

void DMXEuroliteDevice::sendDMXValuesSerialInternal()
{
	static const uint8_t MSG_START = 0x7E;
	static const uint8_t CMD_SEND = 0x06;
	static const uint8_t MSG_END = 0xE7;
	uint16_t len = 512+1;
	uint8_t len_hi = len >> 8;
	uint8_t len_lo = len & 0xFF;

	try
	{
		dmxPort->port->setBreak(true);
		dmxPort->port->setBreak(false);
		dmxPort->writeBytes({MSG_START, CMD_SEND, len_lo, len_hi, 0x00});
		dmxPort->port->write(dmxDataOut, 512);
		dmxPort->writeBytes({MSG_END});
		dmxPort->port->flush();
	}
	catch (serial::IOException e)
	{
		DBG("IO Exception on Eurolite: " << e.what());
	}
	catch (serial::PortNotOpenedException)
	{
		DBG("Port not opened on Eurolite");
	}
	catch (serial::SerialException e)
	{
		DBG("Serial exception: " << e.what());
	}
}
