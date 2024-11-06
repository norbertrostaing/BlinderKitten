/*
  ==============================================================================

    MIDIFeedback.h
    Created: 12 Oct 2020 11:07:59am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
class MIDIInterface;

class MIDIFeedback :
    public BaseItem
{
public:
    MIDIFeedback();
    ~MIDIFeedback();

    MIDIInterface* inter = nullptr;

    enum FeedbackSource { VFADER, VROTARY, VABOVEBUTTON, VBELOWBUTTON, VBUTTON, ENCODER, GRANDMASTER, BLACKOUT, MIDILOCK, BLIND, HL};
    EnumParameter* feedbackSource;
    IntParameter* sourceId;
    IntParameter* sourcePage;
    IntParameter* sourceCol;
    IntParameter* sourceRow;
    IntParameter* sourceNumber;
    BoolParameter* onlyIfCurrentPage;

    enum MidiType { NOTE, CONTROLCHANGE, PITCHWHEEL, TEXT };
    EnumParameter* midiType;
    IntParameter* channel;
    IntParameter* pitchOrNumber;
    Point2DParameter* outputRange7b;
    Point2DParameter* outputRange14b;

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

    enum TextMode { MCU_ENC, MCU_FADER, MCU_ENCANDFADER, XTE_ENC, XTE_FADER, XTE_ENCANDFADER};
    EnumParameter* textMode;
    IntParameter* mackieColumn;

    Point2DParameter* inputRange;

    BoolParameter* learnMode;
    int lastSentValue = -1;
    int lastSentChannel = -1;

    bool isValid;
    bool wasInRange;

    void updateDisplay();
    void onContainerParameterChangedInternal(Parameter*);

    void processFeedback(String address, var value, String origin, bool logOutput);
    void sendText(String text);
    void sendMCUFaderText(int col, String text);
    void sendMCUEncoderText(int col, String text);
    void sendXTEFaderText(int col, String text);
    void sendXTEEncoderText(int col, String text);

    String getLocalAdress();

};