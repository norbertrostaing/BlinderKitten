/*
  ==============================================================================

    ChannelType.h
    Created: 7 Nov 2021 7:40:48pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "../Command/CommandSelectionManager.h"

class StampMapping:
    public BaseItem
{
    public:
    StampMapping(var params = var());
    ~StampMapping();

    String objectType;
    var objectData;

    enum PixelVal {R, G, B, Luma};
    EnumParameter* pixelVal;

    EnumParameter* stampMode;
    TargetParameter* paramType;
    FloatParameter* baseValue;

    Point2DParameter* outRange;

    void onContainerParameterChangedInternal(Parameter*);
    void updateDisplay();

};