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

    enum FeedbackSource { VFADER, VROTARY, VBUTTON};
    EnumParameter* feedbackSource;
    IntParameter* sourceId;
    IntParameter* sourcePage;
    IntParameter* sourceCol;
    IntParameter* sourceRow;
    IntParameter* sourceNumber;

    enum MidiType { NOTE, CONTROLCHANGE, PITCHWHEEL };
    EnumParameter* midiType;
    IntParameter* channel;
    IntParameter* pitchOrNumber;
    Point2DParameter* outputRange;

    IntParameter* onValue;
    IntParameter* offValue;
    IntParameter* onLoadedValue;
    IntParameter* offLoadedValue;
    IntParameter* isGenericValue;

    Point2DParameter* inputRange;

    BoolParameter* learnMode;

    bool isValid;
    bool wasInRange;

    void updateDisplay();
    void onContainerParameterChangedInternal(Parameter*);

    void processFeedback(String address, double value);

};