/*
  ==============================================================================

    Interface.h
    Created: 26 Sep 2020 1:50:32pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
class Object;
class ObjectComponent;
class InterfaceUI;

class Interface :
    public BaseItem
{
public:
    Interface(String name = "Interface");
    virtual ~Interface();

    BoolParameter* logIncomingData;
    BoolParameter* logOutgoingData;


    virtual void sendValuesForObject(Object* o) {}


    virtual ControllableContainer* getInterfaceParams() { return new ControllableContainer("Interface parameters"); }

    virtual InterfaceUI* createUI();
};