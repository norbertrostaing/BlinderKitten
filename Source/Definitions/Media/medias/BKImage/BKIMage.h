/*
  ==============================================================================

    BKImage.h
    Created: 26 Sep 2020 1:51:42pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

//#include "../../Common/CommonIncludes.h"
#include "Definitions/Media/Media.h"

class FixturePatch;

class BKImage :
    public Media
{
public:
    BKImage(var params = var());
    ~BKImage();

    FileParameter* filePath;
    Image imageContent;

    void clearItem() override;
    Colour getColourAtCoord(Point<float>* point) override;
    void onContainerParameterChanged(Parameter* p) override;
    
    String getTypeString() const override { return "Image"; }
    static BKImage* create(var params) { return new BKImage(); };

    //virtual MediaUI* createUI() {return new BKImage(); };
};