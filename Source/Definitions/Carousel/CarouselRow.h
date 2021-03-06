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
#include "CarouselStep.h"

class CarouselRow:
    public BaseItem
{
    public:
    CarouselRow(var params = var());
    ~CarouselRow();

    String objectType;
    var objectData;

    BaseManager<CarouselStep> paramContainer;

    FloatParameter* elementsStart;
    FloatParameter* elementsSpread;

    IntParameter* wings;
    IntParameter* buddying;
    // EnumParameter* curvePresetOrValue;
    // Automation curve;
    // FloatParameter* curveOrigin;
    // IntParameter* presetId;
    // FloatParameter* chaserFade;
    // IntParameter* chaserBuddying;
    // FloatParameter* speed;

    CommandSelectionManager selection;

    HashMap<SubFixture*, double> computedPositions;
    HashMap<SubFixtureChannel*,double> subFixtureChannelOffsets;

    void computeData();

    void onControllableFeedbackUpdate(ControllableContainer*, Controllable*);
    void updateDisplay();


};