/*
  ==============================================================================

    OSCFeedback.h
    Created: 12 Oct 2020 11:07:59am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
class OSCInterface;

class OSCFeedback :
    public BaseItem
{
public:
    OSCFeedback();
    ~OSCFeedback();

    OSCInterface* inter = nullptr;

    enum FeedbackType { VALUE, TEXT };
    EnumParameter* feedbackType;

    enum FeedbackSource { VFADER, VROTARY, VABOVEBUTTON, VBELOWBUTTON, VBUTTON, ENCODER, GRANDMASTER, BLACKOUT, MIDILOCK, BLIND, HL};
    EnumParameter* feedbackSource;
    IntParameter* sourceId;
    IntParameter* sourcePage;
    IntParameter* sourceCol;
    IntParameter* sourceRow;
    IntParameter* sourceNumber;
    BoolParameter* onlyIfCurrentPage;

    StringParameter* outputAddress;
    Point2DParameter* outputRange;

    IntParameter* onValue;
    IntParameter* offValue;
    IntParameter* onLoadedValue;
    IntParameter* offLoadedValue;
    IntParameter* isGenericValue;
    IntParameter* currentCueValue;
    IntParameter* loadedCueValue;

    int lastSentValue = -1;
    int lastSentChannel = -1;

    bool isValid;
    bool wasInRange;

    void updateDisplay();
    void onContainerParameterChangedInternal(Parameter*);

    void processFeedback(String address, var value, String origin, bool logOutput);

    String getLocalAdress();

};