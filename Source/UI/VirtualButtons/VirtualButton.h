/*
  ==============================================================================

    Object.h
    Created: 26 Sep 2020 10:02:32am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class VirtualButton :
    public BaseItem
{
public:
    VirtualButton(var params = var());
    virtual ~VirtualButton();

    String objectType;
    var objectData;

    IntParameter * pageNumber;
    IntParameter * rowNumber;
    IntParameter * colNumber;

    EnumParameter * targetType;
    IntParameter * targetId;
    EnumParameter * cuelistAction;
    EnumParameter * effectAction;
    EnumParameter * carouselAction;

    void onContainerParameterChangedInternal(Parameter* p);
    void updateName();
    void updateDisplay();

    void pressed();
    void released();
    String getBtnText();

    String getTypeString() const override { return objectType; }

    static VirtualButton* create(var params) { return new VirtualButton(params); }
};
