/*
  ==============================================================================

    BKVideo.h
    Created: 26 Sep 2020 1:51:42pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

//#include "../../Common/CommonIncludes.h"
#include "Definitions/Media/Media.h"

class FixturePatch;

class BKVideo :
    public Media
{
public:
    BKVideo(var params = var());
    ~BKVideo();

    FileParameter* filePath;

    void clearItem() override;
    Colour getColourAtCoord(Point<float>* point) override;
    void onContainerParameterChanged(Parameter* p) override;
    
    String getTypeString() const override { return "Video"; }
    static BKVideo* create(var params) { return new BKVideo(); };

    //virtual MediaUI* createUI() {return new BKVideo(); };
};