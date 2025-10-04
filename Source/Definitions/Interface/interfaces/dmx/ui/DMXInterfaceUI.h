/*
  ==============================================================================

    DMXInterfaceUI.h
    Created: 8 Oct 2020 6:59:58pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class DMXInterface;

class DMXInterfaceUI :
    public InterfaceUI
{
public:
    DMXInterfaceUI(DMXInterface* i);
    ~DMXInterfaceUI();

    DMXInterface* dmxInterface;
    
    void controllableFeedbackUpdateInternal(Controllable* c) override;
};
