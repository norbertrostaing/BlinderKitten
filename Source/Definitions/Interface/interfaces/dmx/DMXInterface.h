/*
  ==============================================================================

    DMXInterface.h
    Created: 26 Sep 2020 1:51:42pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

//#include "../../Common/CommonIncludes.h"
#include "Definitions/Interface/InterfaceIncludes.h"
#include "UI/DMXChannelView.h"
#include "DMXMappingManager.h"
#include "Definitions/ChannelFamily/ChannelType/ChannelType.h"

class FixturePatch;

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

    std::unique_ptr<ControllableContainer> thruManager;
    BoolParameter * channelTestingMode;
    FloatParameter* channelTestingFlashValue;
    DMXChannelView* tester = nullptr;

    DMXMappingManager mappingManager;
    Array<FixturePatch* > channelToFixturePatch;
    Array<ChannelType* >channelToChannelType;

    void clearItem() override;

    void onContainerParameterChanged(Parameter* p) override;
    
    void setCurrentDMXDevice(DMXDevice* d);

    void sendDMXValue(int channel, int value);
    void sendDMXValues(int startChannel, Array<int> values);
    void send16BitDMXValue(int startChannel, int value, DMXByteOrder byteOrder);
    void send16BitDMXValues(int startChannel, Array<int> values, DMXByteOrder byteOrder);

    void sendDMXValue(int channel, int value, Array<DMXInterface*>callers);
    void sendDMXValues(int startChannel, Array<int> values, Array<DMXInterface*>callers);
    void send16BitDMXValue(int startChannel, int value, DMXByteOrder byteOrder, Array<DMXInterface*>callers);
    void send16BitDMXValues(int startChannel, Array<int> values, DMXByteOrder byteOrder, Array<DMXInterface*>callers);

    void dmxDeviceConnected() override;
    void dmxDeviceDisconnected() override;

    void dmxDataInChanged(int numChannels, uint8* values) override;
    static void createThruControllable(ControllableContainer* cc);
    
    void afterLoadJSONDataInternal() override;

    void repaintChannels(int chan, int n);

    void dmxChannelInChanged(int num, uint8 val) override;

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