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

    void handleNote(int channel, int pitch, int velocity, String origin);
    void handleCC(int channel, int number, int value, String origin);
    void handlePitchWheel(int channel, int value, String origin);
};