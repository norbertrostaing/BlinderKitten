/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "Media.h"

class MediaManager :
    public BaseManager<Media>
{
public:
    juce_DeclareSingleton(MediaManager, true);

    MediaManager();
    ~MediaManager();

    Factory<Media> factory;

    void addItemInternal(Media* o, var data) override;
    void removeItemInternal(Media* o) override;

    void onContainerParameterChanged(Parameter* p) override;

    // static int sort(Media* A, Media* B) {return (int)A->id->getValue() - (int)B->id->getValue();};

};