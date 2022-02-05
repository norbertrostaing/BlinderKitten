/*
  ==============================================================================

    MIDIInterface.h
    Created: 12 Oct 2020 11:07:40am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class MIDIInterface :
    public Interface,
    public MIDIInputDevice::MIDIInputListener
{
public:
    MIDIInterface();
    ~MIDIInterface();

    MIDIDeviceParameter* deviceParam;

    MIDIMappingManager mappingManager;
    MIDIInputDevice* inputDevice;

    void updateDevices();

    void onContainerParameterChangedInternal(Parameter *) override;

    void noteOnReceived(const int &channel, const int &pitch, const int &velocity) override;
    void noteOffReceived(const int &channel, const int &pitch, const int &velocity) override;
    void controlChangeReceived(const int &channel, const int &number, const int &value) override;

    String getTypeString() const override { return "MIDI"; }
    static MIDIInterface* create(var params) { return new MIDIInterface(); };
};