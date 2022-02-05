/*
  ==============================================================================

    InterfaceUI.h
    Created: 26 Sep 2020 7:47:24pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "../Interface.h"

class InterfaceUI :
    public BaseItemUI<Interface>
{
public:
    InterfaceUI(Interface* item);
    virtual ~InterfaceUI();
};