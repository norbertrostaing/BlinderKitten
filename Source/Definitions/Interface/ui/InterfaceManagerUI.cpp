/*
  ==============================================================================

    InterfaceManagerUI.cpp
    Created: 26 Sep 2020 7:48:12pm
    Author:  bkupe

  ==============================================================================
*/

#include "InterfaceManagerUI.h"

InterfaceManagerUI::InterfaceManagerUI(const String& name) :
    BaseManagerShapeShifterUI(name, InterfaceManager::getInstance())
{
    addExistingItems();
}

InterfaceManagerUI::~InterfaceManagerUI()
{
}

InterfaceUI* InterfaceManagerUI::createUIForItem(Interface* item)
{
    return item->createUI();
}
