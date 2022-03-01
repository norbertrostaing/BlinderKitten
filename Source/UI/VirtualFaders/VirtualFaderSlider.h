/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

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
    EnumParameter * carouselAction;

    void onContainerParameterChangedInternal(Parameter* p);
    void updateName();
    void updateDisplay();

    void moved(float value, String colTargetType, int colTargetId);
    void released();
    String getBtnText(String columnType);

    String getTypeString() const override { return objectType; }

    static VirtualFaderSlider* create(var params) { return new VirtualFaderSlider(params); }
};
