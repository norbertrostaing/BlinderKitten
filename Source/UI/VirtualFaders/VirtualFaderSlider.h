/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Common/CommonIncludes.h"

class VirtualFaderCol;

class VirtualFaderSlider :
    public BaseItem
{
public:
    VirtualFaderSlider(var params = var());
    virtual ~VirtualFaderSlider();

    String objectType;
    var objectData;

    bool isFader = false;
    int validIncrementIndex = 0;

    EnumParameter * targetType;
    IntParameter * targetId;
    StringParameter* customText;
    EnumParameter * cuelistAction;
    EnumParameter * effectAction;
    EnumParameter* carouselAction;
    EnumParameter* mapperAction;
    EnumParameter* trackerAction;
    FloatParameter* maxSpeed;

    ActionManager actionManager;

    void onContainerParameterChangedInternal(Parameter* p);
    void updateName();
    void updateDisplay();
    float getTargetValue();

    void moved(float value, String origin, int incrementIndex, bool isRelative);
    void feedback(var value, String origin);

    void released();
    String getBtnText(String columnType);

    String getTypeString() const override { return objectType; }
    bool isAllowedToMove(String origin, int incrementIndex, float newValue);

    VirtualFaderCol* parentColumn = nullptr;
    bool checkParentColumn();

    static VirtualFaderSlider* create(var params) { return new VirtualFaderSlider(params); }
};
