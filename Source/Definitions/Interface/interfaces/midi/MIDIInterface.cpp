/*
  ==============================================================================

    MIDIInterface.cpp
    Created: 12 Oct 2020 11:07:40am
    Author:  bkupe

  ==============================================================================
*/

#include "Definitions/Interface/InterfaceIncludes.h"
#include "UI/InputPanel.h"
#include "MIDIInterface.h"

MIDIInterface::MIDIInterface() :
    Interface(getTypeString()),
    inputDevice(nullptr),
    outputDevice(nullptr),
    dataContainer("Start sysex content")
{
    deviceParam = new MIDIDeviceParameter("Device");

    addParameter(deviceParam);
    infos = addStringParameter("Infos", "Note informations about your controler here.", "");
    infos->multiline = true;

    autoAdd = addBoolParameter("Auto add", "Create mappings when receive notes, cc or pitchbends", false);

    numBytes = addIntParameter("Start sysex", "Sysex message to send at startup, F0 and F7 are automatically added at the begin and the end, so don't fill them :) .", 0, 0);

    enableMidiClock = addBoolParameter("Midi clock", "If enabled, this interface can receive midi clock", true);
    PPQN = addIntParameter("PPQN", "The number of pulses per quarter note.", 24,4);
    tickDuration = 1.0/24.0;

    addChildControllableContainer(&dataContainer);
    updateBytesParams();

    addChildControllableContainer(&mappingManager);
    addChildControllableContainer(&feedbackManager);
}

MIDIInterface::~MIDIInterface()
{
    if (inputDevice != nullptr) inputDevice->removeMIDIInputListener(this);
    if (outputDevice != nullptr) outputDevice->close();
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

    if (deviceParam->outputDevice != outputDevice) {
        MIDIOutputDevice* newOutput = deviceParam->outputDevice;
        if (outputDevice != nullptr)
        {
            outputDevice->close();
        }

        outputDevice = newOutput;

        if (outputDevice != nullptr)
        {
            outputDevice->open();
            NLOG(niceName, "Now writing to MIDI Device : " << outputDevice->name);
            sendStartupBytes();
        }
    }
}

void MIDIInterface::onContainerParameterChangedInternal(Parameter* p)
{
    if (p == deviceParam)
    {
        updateDevices();
    }
    if (p == numBytes) 
    {
        updateBytesParams();
    }
    if (p == PPQN) {
        tickDuration = 1/ PPQN->floatValue();   
    }
}

void MIDIInterface::sendStartupBytes()
{
    if (numBytes->intValue() == 0) return;
    if (outputDevice == nullptr) return; 

    Array<uint8> data;
    for (auto& c : dataContainer.controllables) data.add(((IntParameter*)c)->intValue());
    outputDevice->sendSysEx(data);
}

void MIDIInterface::noteOnReceived(const int &channel, const int &pitch, const int &velocity)
{
    if (InputPanel::getInstance()->paramInLock->boolValue()) return;
    if (!enabled->boolValue()) {return;}
    if (logIncomingData->boolValue()) NLOG(niceName, "Note On received, channel : " << channel << ", pitch : " << pitch << ", velocity : " << velocity);
    TSLastReceived.set("Note-"+String(channel)+"-"+String(pitch), Time::getApproximateMillisecondCounter());
    bool processed = mappingManager.handleNote(channel, pitch, velocity, niceName);
    if (autoAdd->boolValue() && !processed) {
        MessageManagerLock mmlock;
        MIDIMapping* m = mappingManager.addItem();
        m->midiType->setValueWithData(MIDIMapping::MidiType::NOTE);
        m->channel->setValue(channel);
        m->pitchOrNumber->setValue(pitch);
        m->setNiceName("Note " + String(pitch) + " Channel " + String(channel));
    }
}

void MIDIInterface::noteOffReceived(const int &channel, const int &pitch, const int &velocity)
{
    if (InputPanel::getInstance()->paramInLock->boolValue()) return;
    if (!enabled->boolValue()) { return; }
    if (logIncomingData->boolValue()) NLOG(niceName, "Note Off received, channel : " << channel << ", pitch : " << pitch << ", velocity : " << velocity);
    TSLastReceived.set("Note-" + String(channel) + "-" + String(pitch), Time::getMillisecondCounter());
    bool processed = mappingManager.handleNote(channel, pitch, 0, niceName);
    if (autoAdd->boolValue() && !processed) {
        MessageManagerLock mmlock;
        MIDIMapping* m = mappingManager.addItem();
        m->midiType->setValueWithData(MIDIMapping::MidiType::NOTE);
        m->channel->setValue(channel);
        m->pitchOrNumber->setValue(pitch);
        m->setNiceName("Note " + String(pitch) + " Channel " + String(channel));
    }
}

void MIDIInterface::controlChangeReceived(const int& channel, const int& number, const int& value)
{
    if (InputPanel::getInstance()->paramInLock->boolValue()) return;
    if (!enabled->boolValue()) { return; }
    if (logIncomingData->boolValue()) NLOG(niceName, "Control Change received, channel : " << channel << ", number : " << number << ", value : " << value);
    TSLastReceived.set("CC-" + String(channel) + "-" + String(number), Time::getMillisecondCounter());
    bool processed = mappingManager.handleCC(channel, number, value, niceName);
    if (autoAdd->boolValue() && !processed) {
        MessageManagerLock mmlock;
        MIDIMapping* m = mappingManager.addItem();
        m->midiType->setValueWithData(MIDIMapping::MidiType::CONTROLCHANGE);
        m->channel->setValue(channel);
        m->pitchOrNumber->setValue(number);
        m->setNiceName("Control change " + String(number) + " Channel " + String(channel));
    }
}

void MIDIInterface::pitchWheelReceived(const int& channel, const int& value)
{
    if (InputPanel::getInstance()->paramInLock->boolValue()) return;
    if (!enabled->boolValue()) { return; }
    if (logIncomingData->boolValue()) NLOG(niceName, "Pitch wheel received, channel : " << channel << ", value : " << value);
    TSLastReceived.set("PW-" + String(channel), Time::getMillisecondCounter());
    bool processed = mappingManager.handlePitchWheel(channel, value, niceName);
    if (autoAdd->boolValue() && !processed) {
        MessageManagerLock mmlock;
        MIDIMapping* m = mappingManager.addItem();
        m->midiType->setValueWithData(MIDIMapping::MidiType::PITCHWHEEL);
        m->channel->setValue(channel);
        m->setNiceName("Pitch wheel " + String(channel));
    }
}

void MIDIInterface::midiClockReceived()
{
    if (!enabled->boolValue()) { return; }
    if (!enableMidiClock->boolValue()) {return; }
    clockListeners.call(&ClockListener::midiClockTick);
}

void MIDIInterface::midiStartReceived()
{
    if (!enabled->boolValue()) { return; }
    if (!enableMidiClock->boolValue()) { return; }
    clockListeners.call(&ClockListener::midiClockStart);
}

void MIDIInterface::midiContinueReceived()
{
    if (!enabled->boolValue()) { return; }
    if (!enableMidiClock->boolValue()) { return; }
    clockListeners.call(&ClockListener::midiClockContinue);
}

void MIDIInterface::midiStopReceived()
{
    if (!enabled->boolValue()) { return; }
    if (!enableMidiClock->boolValue()) { return; }
    clockListeners.call(&ClockListener::midiClockStop);
}

void MIDIInterface::feedback(String address, var value, String origin = "")
{
    if (!enabled->boolValue()) return;  
    bool logOutput = logOutgoingData->boolValue();
    feedbackManager.feedback(address, value, origin, logOutput);
}


void MIDIInterface::updateBytesParams()
{
    while (dataContainer.controllables.size() > numBytes->intValue())
    {
        dataContainer.controllables[dataContainer.controllables.size() - 1]->remove();
    }

    while (dataContainer.controllables.size() < numBytes->intValue())
    {
        String index = String(dataContainer.controllables.size());
        IntParameter* p = new IntParameter("#" + index, "Data for the byte #" + index, 0, 0, 255);
        p->hexMode = true;
        dataContainer.addParameter(p); // after hexMode
    }

}
