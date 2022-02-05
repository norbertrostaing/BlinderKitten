/*
  ==============================================================================

    InterfaceManager.h
    Created: 26 Sep 2020 1:50:28pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "Interface.h"

class InterfaceManager :
    public BaseManager<Interface>
{
public:
    juce_DeclareSingleton(InterfaceManager, true)

    InterfaceManager();
    ~InterfaceManager();

    Factory<Interface> factory;
};