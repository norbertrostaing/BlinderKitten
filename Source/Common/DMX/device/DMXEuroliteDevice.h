
#pragma once

class DMXEuroliteDevice :
	public DMXSerialDevice
{
public:
	DMXEuroliteDevice();
	~DMXEuroliteDevice();

	void setPortConfig() override;
	void sendDMXValuesSerialInternal() override;
};
