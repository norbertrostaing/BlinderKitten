/*
  ==============================================================================

    MIDIFeedbackManager.cpp
    Created: 12 Oct 2020 11:07:45am
    Author:  bkupe

  ==============================================================================
*/

#include "Interface/InterfaceIncludes.h"
#include "MIDIFeedbackManager.h"
#include "MIDIFeedback.h"

MIDIFeedbackManager::MIDIFeedbackManager() :
    BaseManager("Feedbacks")
{
}

MIDIFeedbackManager::~MIDIFeedbackManager()
{
}

void MIDIFeedbackManager::controllableFeedbackUpdate(ControllableContainer* cc, Controllable* c)
{
    rebuildLibrary();
}

void MIDIFeedbackManager::feedback(String address, var value, String origin, bool logOutput)
{
    if (feedbackLibrary.contains(address)) {
        for (MIDIFeedback* f : feedbackLibrary.getReference(address)) {
            f->processFeedback(address, value, origin, logOutput);
        }
    }
}

void MIDIFeedbackManager::rebuildLibrary()
{
    isComputing.enter();
    feedbackLibrary.clear();
    for (MIDIFeedback* f : items) {
        String ad = f->getLocalAdress();
        LOG("coucou");
        LOG(ad);
        if (!feedbackLibrary.contains(ad)) {
            feedbackLibrary.set(ad, Array<MIDIFeedback*>());
        }
        feedbackLibrary.getReference(ad).add(f);
    }
    isComputing.exit();
}

void MIDIFeedbackManager::afterLoadJSONDataInternal()
{
    rebuildLibrary();
}

