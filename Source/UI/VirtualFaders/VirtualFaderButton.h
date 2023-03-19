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
//#include "VirtualFaderCol.h"
class VirtualFaderCol;

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
    StringParameter* customText;

    EnumParameter * cuelistAction;
    EnumParameter * effectAction;
    EnumParameter* carouselAction;
    EnumParameter* mapperAction;
    FloatParameter* cueId;

    ActionManager actionManager;
    VirtualFaderCol* parentColumn = nullptr;

    void onContainerParameterChangedInternal(Parameter* p);
    void updateName();
    void updateDisplay();

    void pressed();
    void released();
    String getBtnText(String columnType);

    bool checkParentColumn();

    String getTypeString() const override { return objectType; };
    static VirtualFaderButton* create(var params) { return new VirtualFaderButton(params); }
};
