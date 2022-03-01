#include "VirtualFaderCol.h"
#include "VirtualFaderColManager.h"

/*
  ==============================================================================

    ObjectManager.cpp
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

juce_ImplementSingleton(VirtualFaderColManager);

VirtualFaderColManager::VirtualFaderColManager() :
    BaseManager("VirtualFaderCol")
{
    itemDataType = "VirtualFaderCol";
    saveAndLoadRecursiveData = true;
    // selectItemWhenCreated = true;
    // autoReorderOnAdd = true;
}

VirtualFaderColManager::~VirtualFaderColManager()
{
    // stopThread(1000);
}


void VirtualFaderColManager::addItemInternal(VirtualFaderCol* o, var data)
{
    reorderItems();
}

void VirtualFaderColManager::removeItemInternal(VirtualFaderCol* o)
{
}

void VirtualFaderColManager::onContainerParameterChanged(Parameter* p)
{
}

