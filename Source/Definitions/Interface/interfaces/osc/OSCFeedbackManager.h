/*
  ==============================================================================

    OSCFeedbackManager.h
    Created: 12 Oct 2020 11:07:45am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class OSCFeedbackManager :
    public BaseManager<OSCFeedback>
{
public:
    OSCFeedbackManager();
    ~OSCFeedbackManager();

    void controllableFeedbackUpdate(ControllableContainer* cc, Controllable* c) override;
    void feedback(String address, var value, String origin, bool logOutput);
    void removeItemInternal(OSCFeedback* o) override;

    HashMap<String, Array<OSCFeedback*>> feedbackLibrary;
    CriticalSection isComputing;
    void rebuildLibrary();

    void afterLoadJSONDataInternal();
};