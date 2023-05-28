/*
  ==============================================================================

    MIDIFeedbackManager.cpp
    Created: 12 Oct 2020 11:07:45am
    Author:  bkupe

  ==============================================================================
*/

#include "Interface/InterfaceIncludes.h"

MIDIFeedbackManager::MIDIFeedbackManager() :
    BaseManager("Feedbacks")
{
}

MIDIFeedbackManager::~MIDIFeedbackManager()
{
}

void MIDIFeedbackManager::handleNote(int channel, int pitch, int velocity, String origin)
{
    for (auto& i : items) i->handleNote(channel, pitch, velocity, origin);
}

void MIDIFeedbackManager::handleCC(int channel, int number, int value, String origin)
{
    for (auto& i : items) i->handleCC(channel, number, value, origin);
}

void MIDIFeedbackManager::handlePitchWheel(int channel, int value, String origin)
{
    for (auto& i : items) i->handlePitchWheel(channel, value, origin);
}
