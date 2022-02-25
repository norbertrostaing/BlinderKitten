#include "VirtualButton.h"
#include "VirtualButtonManager.h"

/*
  ==============================================================================

    ObjectManager.cpp
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

juce_ImplementSingleton(VirtualButtonManager);

VirtualButtonManager::VirtualButtonManager() :
    BaseManager("VirtualButton")
{
    itemDataType = "VirtualButton";
    // selectItemWhenCreated = true;
    // autoReorderOnAdd = true;
}

VirtualButtonManager::~VirtualButtonManager()
{
    // stopThread(1000);
}


void VirtualButtonManager::addItemInternal(VirtualButton* o, var data)
{
    reorderItems();
}

void VirtualButtonManager::removeItemInternal(VirtualButton* o)
{
}

void VirtualButtonManager::onContainerParameterChanged(Parameter* p)
{
}

