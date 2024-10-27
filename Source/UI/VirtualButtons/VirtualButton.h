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

class VirtualButton :
    public BaseItem
{
public:
    VirtualButton(var params = var());
    virtual ~VirtualButton();

    String objectType;
    var objectData;

    enum ButtonStatus{BTN_UNASSIGNED, BTN_ON, BTN_OFF, BTN_ON_LOADED, BTN_OFF_LOADED, BTN_GENERIC, BTN_CURRENTCUE, BTN_LOADEDCUE};
    ButtonStatus currentStatus;

    IntParameter * pageNumber;
    IntParameter * rowNumber;
    IntParameter * colNumber;
    StringParameter * customText;
    BoolParameter * customColorEnabled;
    ColorParameter * customColor;

    EnumParameter * targetType;
    IntParameter * targetId;
    EnumParameter * cuelistAction;
    EnumParameter * effectAction;
    EnumParameter* carouselAction;
    EnumParameter* mapperAction;
    EnumParameter* trackerAction;
    FloatParameter* cueId;

    ActionManager actionManager;

    void onContainerParameterChangedInternal(Parameter* p);
    void updateName();
    void updateDisplay();

    void pressed();
    void released();
    String getBtnText();
    bool useCustomColor();
    juce::Colour getCustomColor();

    void updateStatus(bool forceRefresh = false);
    void feedback(ButtonStatus value);
    void feedback(String value);

    String getTypeString() const override { return objectType; }

    static VirtualButton* create(var params) { return new VirtualButton(params); }
};
