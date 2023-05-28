/*
  ==============================================================================

    MIDIInterface.cpp
    Created: 12 Oct 2020 11:07:40am
    Author:  bkupe

  ==============================================================================
*/

#include "Definitions/Interface/InterfaceIncludes.h"

MIDIInterface::MIDIInterface() :
    Interface(getTypeString()),
    inputDevice(nullptr)
{
    deviceParam = new MIDIDeviceParameter("Device");
    addParameter(deviceParam);

    addChildControllableContainer(&mappingManager);
    addChildControllableContainer(&feedbackManager);
}

MIDIInterface::~MIDIInterface()
{
    if (inputDevice != nullptr) inputDevice->removeMIDIInputListener(this);

}

void MIDIInterface::updateDevices()
{
    if (isClearing) return;

    if (deviceParam->inputDevice != inputDevice)
    {
        if (inputDevice != nullptr) inputDevice->removeMIDIInputListener(this);

        inputDevice = deviceParam->inputDevice;

        if (inputDevice != nullptr)
        {
            inputDevice->addMIDIInputListener(this);
            NLOG(niceName, "Now listening to MIDI Device : " << inputDevice->name);
        }
    }
}

void MIDIInterface::onContainerParameterChangedInternal(Parameter* p)
{
    if (p == deviceParam)
    {
        updateDevices();
    }
}

void MIDIInterface::noteOnReceived(const int &channel, const int &pitch, const int &velocity)
{
    if (!enabled->boolValue()) {return;}
    if (logIncomingData->boolValue()) NLOG(niceName, "Note On received, channel : " << channel << ", pitch : " << pitch << ", velocity : " << velocity);
    mappingManager.handleNote(channel, pitch, velocity, niceName);
}

void MIDIInterface::noteOffReceived(const int &channel, const int &pitch, const int &velocity)
{
    if (!enabled->boolValue()) { return; }
    if (logIncomingData->boolValue()) NLOG(niceName, "Note Off received, channel : " << channel << ", pitch : " << pitch << ", velocity : " << velocity);
    mappingManager.handleNote(channel, pitch, 0, niceName);
}

void MIDIInterface::controlChangeReceived(const int &channel, const int &number, const int &value)
{
    if (!enabled->boolValue()) { return; }
    if (logIncomingData->boolValue()) NLOG(niceName, "Control Change received, channel : " << channel << ", number : " << number << ", value : " << value);
    mappingManager.handleCC(channel, number, value, niceName);
}

void MIDIInterface::pitchWheelReceived(const int& channel, const int& value)
{
    if (!enabled->boolValue()) { return; }
    if (logIncomingData->boolValue()) NLOG(niceName, "Pitch wheel received, channel : " << channel << ", value : " << value);
    mappingManager.handlePitchWheel(channel, value, niceName);
}

void MIDIInterface::feedback(String address, double value)
{
    for (int i = 0; i < feedbackManager.items.size(); i++) {
        MIDIFeedback* f = feedbackManager.items[i];
        f->processFeedback(address, value);
    }
}
