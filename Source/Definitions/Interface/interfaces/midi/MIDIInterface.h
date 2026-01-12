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
    MIDIFeedbackManager feedbackManager;
    MIDIInputDevice* inputDevice;
    MIDIOutputDevice* outputDevice;

    BoolParameter* autoAdd;

    IntParameter* numBytes;
    ControllableContainer dataContainer;

    BoolParameter* enableMidiClock;
    IntParameter* PPQN;
    double tickDuration;

    BoolParameter* enableMidiTimecode;

    StringParameter* infos;
    HashMap<String, juce::uint32> TSLastReceived;
    HashMap<String, int> delayedValue;

    void updateDevices();
    void updateBytesParams();
    void onContainerParameterChangedInternal(Parameter *) override;

    void sendStartupBytes();

    void noteOnReceived(const int &channel, const int &pitch, const int &velocity) override;
    void noteOffReceived(const int &channel, const int &pitch, const int &velocity) override;
    void controlChangeReceived(const int& channel, const int& number, const int& value) override;
    void pitchWheelReceived(const int& channel, const int& value) override;
    void midiClockReceived() override;
    void midiStartReceived() override;
    void midiStopReceived() override;
    void midiContinueReceived() override;

    void fullFrameTimecodeReceived(const MidiMessage&) override;
    void quarterFrameTimecodeReceived(const MidiMessage&) override;


    void feedback(String address, var value, String origin);

    String getTypeString() const override { return "MIDI"; }
    static MIDIInterface* create(var params) { return new MIDIInterface(); };

    class ClockListener {
    public:
        /** Destructor. */
        virtual ~ClockListener() {}
        virtual void midiClockTick() {};
        virtual void midiClockStart() {};
        virtual void midiClockContinue() {};
        virtual void midiClockStop() {};
    };

    ListenerList<ClockListener> clockListeners;

    void addClockListener(ClockListener* newListener) { clockListeners.add(newListener); }
    void removeClockListener(ClockListener* listener) { clockListeners.remove(listener); }

    class TimecodeListener {
    public:
        /** Destructor. */
        virtual ~TimecodeListener() {}
        virtual void midiTimecodeUpdated(int tc) {};
    };

    std::array<uint8_t, 8> qf = { 0,0,0,0,0,0,0,0 };
    uint8_t qfCycleMask = 0; // reset à chaque cycle complet
    int lastFrameSent = -1;      // filtre doublons

    ListenerList<TimecodeListener> timecodeListeners;

    void addTimecodeListener(TimecodeListener* newListener) { timecodeListeners.add(newListener); }
    void removeTimecodeListener(TimecodeListener* listener) { timecodeListeners.remove(listener); }





};
