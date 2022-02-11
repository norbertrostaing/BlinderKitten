#include "ChannelFamily.h"
#include "ChannelFamilyManager.h"
/*
  ==============================================================================

    ObjectManager.cpp
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

juce_ImplementSingleton(ChannelFamilyManager);


ChannelFamilyManager::ChannelFamilyManager() :
    BaseManager("Channel Families")    {
    itemDataType = "ChannelFamily";
    selectItemWhenCreated = true;
       
    // managerFactory = &factory;

    // gridThumbSize = addIntParameter("Thumb Size", "Size of thumbnails in grid view", 96, 32, 256);
    // defaultFlashValue = addFloatParameter("Flash Value", "Flash Value", .5f, 0, 1);
    // blackOut = addBoolParameter("Black Out", "Force 0 on all computed values", false);
    // filterActiveInScene = addBoolParameter("Show Only active", "Show only active objects in scene", false);
    //lockUI = addBoolParameter("Lock UI", "If checked, all objects will be locked", false);
    // startThread();

}

ChannelFamilyManager::~ChannelFamilyManager()
{
    // stopThread(1000);
}


void ChannelFamilyManager::addItemInternal(ChannelFamily* o, var data)
{
    // o->addChannelFamilyListener(this);
    // if (!isCurrentlyLoadingData) o->globalID->setValue(getFirstAvailableObjectID(o));
}

void ChannelFamilyManager::removeItemInternal(ChannelFamily* o)
{
    // o->removeObjectListener(this);
}


void ChannelFamilyManager::onContainerParameterChanged(Parameter* p)
{
   // if (p == lockUI) for (auto& i : items) i->isUILocked->setValue(lockUI->boolValue());
}

