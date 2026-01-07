/*
  ==============================================================================

    OSCFeedbackManager.cpp
    Created: 12 Oct 2020 11:07:45am
    Author:  bkupe

  ==============================================================================
*/

#include "Interface/InterfaceIncludes.h"
#include "OSCFeedbackManager.h"
#include "OSCFeedback.h"

OSCFeedbackManager::OSCFeedbackManager() :
    BaseManager("Feedbacks")
{
}

OSCFeedbackManager::~OSCFeedbackManager()
{
}

void OSCFeedbackManager::controllableFeedbackUpdate(ControllableContainer* cc, Controllable* c)
{
    rebuildLibrary();
}

void OSCFeedbackManager::feedback(String address, var value, String origin, bool logOutput)
{
    if (feedbackLibrary.contains(address)) {
        for (OSCFeedback* f : feedbackLibrary.getReference(address)) {
            f->processFeedback(address, value, origin, logOutput);
        }
    }
}

void OSCFeedbackManager::removeItemInternal(OSCFeedback* o)
{
    feedbackLibrary.removeValue(o);
}

void OSCFeedbackManager::rebuildLibrary()
{
    isComputing.enter();
    feedbackLibrary.clear();
    for (OSCFeedback* f : items) {
        String ad = f->getLocalAdress();
        if (!feedbackLibrary.contains(ad)) {
            feedbackLibrary.set(ad, Array<OSCFeedback*>());
        }
        feedbackLibrary.getReference(ad).add(f);
    }
    isComputing.exit();
}

void OSCFeedbackManager::afterLoadJSONDataInternal()
{
    rebuildLibrary();
}

