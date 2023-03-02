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

class VirtualFaderButton :
    public BaseItem
{
public:
    VirtualFaderButton(var params = var());
    virtual ~VirtualFaderButton();

    String objectType;
    var objectData;

    EnumParameter * targetType;
    IntParameter * targetId;

    EnumParameter * cuelistAction;
    EnumParameter * effectAction;
    EnumParameter* carouselAction;
    EnumParameter* mapperAction;
    FloatParameter* cueId;

    ActionManager actionManager;

    void onContainerParameterChangedInternal(Parameter* p);
    void updateName();
    void updateDisplay();

    void pressed(String colTargetType, int colTargetId);
    void released(String colTargetType, int colTargetId);
    String getBtnText(String columnType);

    String getTypeString() const override { return objectType; }

    static VirtualFaderButton* create(var params) { return new VirtualFaderButton(params); }
};
