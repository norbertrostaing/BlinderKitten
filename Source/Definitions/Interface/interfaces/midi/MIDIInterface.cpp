/*
  ==============================================================================

    MIDIInterface.cpp
    Created: 12 Oct 2020 11:07:40am
    Author:  bkupe

  ==============================================================================
*/

MIDIInterface::MIDIInterface() :
    Interface(getTypeString()),
    inputDevice(nullptr)
{
    deviceParam = new MIDIDeviceParameter("Device");
    addParameter(deviceParam);

    addChildControllableContainer(&mappingManager);
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
    if (logIncomingData->boolValue()) NLOG(niceName, "Note On received, channel : " << channel << ", pitch : " << pitch << ", velocity : " << velocity);
    mappingManager.handleNote(channel, pitch, velocity);
}

void MIDIInterface::noteOffReceived(const int &channel, const int &pitch, const int &velocity)
{
    if (logIncomingData->boolValue()) NLOG(niceName, "Note Off received, channel : " << channel << ", pitch : " << pitch << ", velocity : " << velocity);
    mappingManager.handleNote(channel, pitch, 0);
}

void MIDIInterface::controlChangeReceived(const int &channel, const int &number, const int &value)
{
    if (logIncomingData->boolValue()) NLOG(niceName, "Control Change received, channel : " << channel << ", number : " << number << ", value : " << value);
    mappingManager.handleCC(channel, number, value);
}
