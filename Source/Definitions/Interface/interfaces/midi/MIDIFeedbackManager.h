/*
  ==============================================================================

    MIDIFeedbackManager.h
    Created: 12 Oct 2020 11:07:45am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class MIDIFeedbackManager :
    public BaseManager<MIDIFeedback>
{
public:
    MIDIFeedbackManager();
    ~MIDIFeedbackManager();

    void controllableFeedbackUpdate(ControllableContainer* cc, Controllable* c) override;
    void feedback(String address, var value, String origin, bool logOutput);

    HashMap<String, Array<MIDIFeedback*>> feedbackLibrary;
    CriticalSection isComputing;
    void rebuildLibrary();

    void afterLoadJSONDataInternal() override;
};
