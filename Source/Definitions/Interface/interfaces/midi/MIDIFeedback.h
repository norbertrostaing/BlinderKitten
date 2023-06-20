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

    enum FeedbackSource { VFADER, VROTARY, VABOVEBUTTON, VBELOWBUTTON, VBUTTON, ENCODER};
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

    BoolParameter* differentChannels;
    IntParameter* onValue;
    IntParameter* offValue;
    IntParameter* onLoadedValue;
    IntParameter* offLoadedValue;
    IntParameter* isGenericValue;
    IntParameter* currentCueValue;
    IntParameter* loadedCueValue;
    IntParameter* onChannel;
    IntParameter* offChannel;
    IntParameter* onLoadedChannel;
    IntParameter* offLoadedChannel;
    IntParameter* isGenericChannel;
    IntParameter* currentCueChannel;
    IntParameter* loadedCueChannel;


    Point2DParameter* inputRange;

    BoolParameter* learnMode;
    int lastSentValue = -1;

    bool isValid;
    bool wasInRange;

    void updateDisplay();
    void onContainerParameterChangedInternal(Parameter*);

    void processFeedback(String address, double value, String origin);

};