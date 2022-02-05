/*
  ==============================================================================

    DMXInterface.h
    Created: 26 Sep 2020 1:51:42pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "../../Common/CommonIncludes.h"

class DMXInterface :
    public Interface,
    public DMXDevice::DMXDeviceListener
{
public:
    DMXInterface();
    ~DMXInterface();

    enum DMXByteOrder { BIT8, MSB, LSB };

    EnumParameter* dmxType;
    std::unique_ptr<DMXDevice> dmxDevice;
    BoolParameter* dmxConnected;

    BoolParameter * channelTestingMode;
    FloatParameter* channelTestingFlashValue;

    void clearItem() override;

    void onContainerParameterChanged(Parameter* p) override;
    
    void setCurrentDMXDevice(DMXDevice* d);

    void sendDMXValue(int channel, int value);
    void sendDMXValues(int startChannel, Array<int> values);
    void send16BitDMXValue(int startChannel, int value, DMXByteOrder byteOrder);
    void send16BitDMXValues(int startChannel, Array<int> values, DMXByteOrder byteOrder);

    void dmxDeviceConnected() override;
    void dmxDeviceDisconnected() override;

    void dmxDataInChanged(int numChannels, uint8* values) override;

    class DMXParams : public ControllableContainer
    {
    public:
        DMXParams();
        IntParameter* startChannel;
    };

    ControllableContainer* getInterfaceParams() override { return new DMXParams(); }

    // void sendValuesForObject(Object* o) override;

    String getTypeString() const override { return "DMX"; }
    static DMXInterface* create(var params) { return new DMXInterface(); };

    virtual InterfaceUI* createUI() override;
};