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

    EnumParameter * targetType;
    IntParameter * targetId;
    EnumParameter * cuelistAction;
    EnumParameter * effectAction;
    EnumParameter* carouselAction;
    EnumParameter* mapperAction;
    FloatParameter* maxSpeed;

    ActionManager actionManager;
    VirtualFaderCol* parentColumn = nullptr;

    void onContainerParameterChangedInternal(Parameter* p);
    void updateName();
    void updateDisplay();
    float getTargetValue(String colTargetType, int colTargetId);

    void moved(float value, String colTargetType, int colTargetId, String origin);
    void released();
    String getBtnText(String columnType);

    String getTypeString() const override { return objectType; }
    bool checkParentColumn();
    bool isAllowedToMove(String origin, float newValue);

    static VirtualFaderSlider* create(var params) { return new VirtualFaderSlider(params); }
};
