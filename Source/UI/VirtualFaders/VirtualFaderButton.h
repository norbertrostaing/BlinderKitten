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

    enum ButtonStatus { BTN_UNASSIGNED, BTN_ON, BTN_OFF, BTN_ON_LOADED, BTN_OFF_LOADED, BTN_GENERIC, BTN_CURRENTCUE, BTN_LOADEDCUE};
    ButtonStatus currentStatus;

    EnumParameter * targetType;
    IntParameter * targetId;
    StringParameter* customText;

    EnumParameter * cuelistAction;
    EnumParameter * effectAction;
    EnumParameter* carouselAction;
    EnumParameter* mapperAction;
    EnumParameter* trackerAction;
    FloatParameter* cueId;

    ActionManager actionManager;
    VirtualFaderCol* parentColumn = nullptr;

    void onContainerParameterChangedInternal(Parameter* p) override;
    void updateName();
    void updateDisplay();

    void pressed();
    void released();
    String getBtnText(String columnType);

    bool checkParentColumn();

    void feedback(ButtonStatus value);
    void feedback(String text);
    void updateStatus(bool forceRefresh = false);

    String getTypeString() const override { return objectType; };
    static VirtualFaderButton* create(var params) { return new VirtualFaderButton(params); }
};
