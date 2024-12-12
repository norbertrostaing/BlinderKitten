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
#include "EffectParam.h"
using namespace siv;

class EffectRow:
    public BaseItem
{
    public:
    EffectRow(var params = var());
    ~EffectRow();

    String objectType;
    var objectData;

    ControllableContainer curveContainer;
    BaseManager<EffectParam> paramContainer;

    EnumParameter* curvePresetOrValue;
    Automation curve;
    FloatParameter* curveOrigin;
    IntParameter* presetId;
    FloatParameter* chaserFade;
    IntParameter* chaserBuddying;
    FloatParameter* speed;
    EnumParameter* direction;

    CommandSelectionManager selection;

    HashMap<SubFixture*, double> computedPositions;
    CriticalSection isComputing;

    PerlinNoise perlinNoise;

    double maxOffset = 0;

    Effect* parentEffect = nullptr;
    void checkParentEffect();

    void computeData();

    void onControllableFeedbackUpdate(ControllableContainer*, Controllable*);
    void updateDisplay();


};