/*
  ==============================================================================

    SolidColor.h
    Created: 26 Sep 2020 1:51:42pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

//#include "../../Common/CommonIncludes.h"
#include "Definitions/Media/Media.h"

class FixturePatch;

class SolidColor :
    public Media
{
public:
    SolidColor(var params = var());
    ~SolidColor();

    ColorParameter* color;

    void clearItem() override;
    Colour getColourAtCoord(Point<float>* point, int pixelsAround) override;
    void onContainerParameterChanged(Parameter* p) override;
    
    String getTypeString() const override { return "SolidColor"; }
    static SolidColor* create(var params) { return new SolidColor(); };

    //virtual MediaUI* createUI() {return new SolidColor(); };
};