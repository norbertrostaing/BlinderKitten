/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "../Command/CommandSelectionManager.h"

class Media;

class MediaUI :
    public BaseItemUI<Media>
{
public:
    MediaUI(Media* item);
    virtual ~MediaUI();
};


class Media :
    public BaseItem
{
public:
    Media(var params = var());
    virtual ~Media();
    CriticalSection useImageData;

    String objectType;
    var objectData;

    IntParameter* id;
    int registeredId = 0;
    StringParameter* userName;
    Image image;
    bool continuousRepaint = false;
    void onContainerParameterChangedInternal(Parameter* p);
    void updateName();

    virtual Colour getColourAtCoord(Point<float>* point, int pixelsAround){return Colour(0,0,0);};

    String getTypeString() const override { return objectType; }

    static Media* create(var params) { return new Media(params); }
};
