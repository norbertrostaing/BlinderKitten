/*
  ==============================================================================

    InterfaceManagerUI.h
    Created: 26 Sep 2020 7:48:12pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "../InterfaceManager.h"
#include "InterfaceUI.h"

class InterfaceManagerUI :
    public BaseManagerShapeShifterUI<InterfaceManager, Interface, InterfaceUI>
{
public:
    InterfaceManagerUI(const String &name);
    ~InterfaceManagerUI();

    InterfaceUI* createUIForItem(Interface* item) override;

    static InterfaceManagerUI* create(const String& name) { return new InterfaceManagerUI(name); }
};