/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "VirtualFaderButton.h"
#include "VirtualFaderSlider.h"

class VirtualFaderCol :
    public BaseItem
{
public:
    VirtualFaderCol(var params = var());
    virtual ~VirtualFaderCol();

    String objectType;
    var objectData;

    IntParameter * pageNumber;
    IntParameter * colNumber;

    EnumParameter * targetType;
    IntParameter * targetId;

    BaseManager<VirtualFaderSlider> rotaries;
    BaseManager<VirtualFaderButton> aboveButtons;
    VirtualFaderSlider fader;
    BaseManager<VirtualFaderButton> belowButtons;

    void onContainerParameterChangedInternal(Parameter* p);
    void updateName();
    void updateDisplay();

    void pressed();
    void released();
    String getBtnText();

    String getTypeString() const override { return objectType; }

    static VirtualFaderCol* create(var params) { return new VirtualFaderCol(params); }
};
