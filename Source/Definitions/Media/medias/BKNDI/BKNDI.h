/*
  ==============================================================================

    BKNDI.h
    Created: 26 Sep 2020 1:51:42pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

//#include "../../Common/CommonIncludes.h"
#include "Definitions/Media/Media.h"

class FixturePatch;

class BKNDI :
    public Media
{
public:
    BKNDI(var params = var());
    ~BKNDI();

    ColorParameter* color;

    void clearItem() override;
    Colour getColourAtCoord(Point<float>* point, int pixelsAround) override;
    void onContainerParameterChanged(Parameter* p) override;
    
    String getTypeString() const override { return "NDI"; }
    static BKNDI* create(var params) { return new BKNDI(); };

    //virtual MediaUI* createUI() {return new BKNDI(); };
};