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

class BaseManagerSlider :
    public BaseManager<VirtualFaderSlider>
{
    public:
    BaseManagerSlider(const String& name);
    ~BaseManagerSlider() {};

    void addItemInternal(VirtualFaderSlider*, var data);
    void removeItemInternal(VirtualFaderSlider*);
    void setItemIndex(VirtualFaderSlider* item, int newIndex, bool addToUndo = true);
};

class BaseManagerButton :
    public BaseManager<VirtualFaderButton>
{
    public:
    BaseManagerButton(const String& name);
    ~BaseManagerButton() {};

    void addItemInternal(VirtualFaderButton*, var data);
    void removeItemInternal(VirtualFaderButton*);
    void setItemIndex(VirtualFaderButton* item, int newIndex, bool addToUndo = true);
};

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

    BaseManagerSlider rotaries;
    BaseManagerButton aboveButtons;
    VirtualFaderSlider fader;
    BaseManagerButton belowButtons;

    void onContainerParameterChangedInternal(Parameter* p);
    void updateName();
    void updateDisplay();

    void pressed();
    void released();
    String getTargetName();
    String getTargetType();

    String getTypeString() const override { return objectType; }
    void loadJSONDataInternal(var data) override;
    static VirtualFaderCol* create(var params) { return new VirtualFaderCol(params); }
};

