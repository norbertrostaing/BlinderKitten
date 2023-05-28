/*
  ==============================================================================

    MIDIFeedback.h
    Created: 12 Oct 2020 11:07:59am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class MIDIFeedback :
    public BaseItem
{
public:
    MIDIFeedback();
    ~MIDIFeedback();

    enum FeedbackSource { VFADER, VROTARY};
    EnumParameter* feedbackSource;
    IntParameter* sourceId;
    IntParameter* sourcePage;
    IntParameter* sourceCol;
    IntParameter* sourceNumber;

    enum MidiType { NOTE, CONTROLCHANGE, PITCHWHEEL };
    EnumParameter* midiType;
    IntParameter* channel;
    IntParameter* pitchOrNumber;
    Point2DParameter* outputRange;

    Point2DParameter* inputRange;

    BoolParameter* learnMode;

    bool isValid;
    bool wasInRange;

    void handleNote(int channel, int pitch, int velocity, String origin);
    void handleCC(int channel, int number, int value, String origin);
    void handlePitchWheel(int channel, int value, String origin);
    void handleValue(float value, String origin);
    void updateDisplay();
    void onContainerParameterChangedInternal(Parameter*);

    void processFeedback(String address, double value);

};