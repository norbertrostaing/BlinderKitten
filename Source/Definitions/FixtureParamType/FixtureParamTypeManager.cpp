#include "FixtureParamType.h"
#include "FixtureParamTypeManager.h"
/*
  ==============================================================================

    ObjectManager.cpp
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

juce_ImplementSingleton(FixtureParamTypeManager);


FixtureParamTypeManager::FixtureParamTypeManager() :
    BaseManager("Channel Families")    {
    itemDataType = "FixtureParamType";
    selectItemWhenCreated = true;
       
    // managerFactory = &factory;

    // gridThumbSize = addIntParameter("Thumb Size", "Size of thumbnails in grid view", 96, 32, 256);
    // defaultFlashValue = addFloatParameter("Flash Value", "Flash Value", .5f, 0, 1);
    // blackOut = addBoolParameter("Black Out", "Force 0 on all computed values", false);
    // filterActiveInScene = addBoolParameter("Show Only active", "Show only active objects in scene", false);
    //lockUI = addBoolParameter("Lock UI", "If checked, all objects will be locked", false);
    // startThread();

}

FixtureParamTypeManager::~FixtureParamTypeManager()
{
    // stopThread(1000);
}


void FixtureParamTypeManager::addItemInternal(FixtureParamType* o, var data)
{
    // o->addFixtureParamTypeListener(this);
    // if (!isCurrentlyLoadingData) o->globalID->setValue(getFirstAvailableObjectID(o));
}

void FixtureParamTypeManager::removeItemInternal(FixtureParamType* o)
{
    // o->removeObjectListener(this);
}


void FixtureParamTypeManager::onContainerParameterChanged(Parameter* p)
{
   // if (p == lockUI) for (auto& i : items) i->isUILocked->setValue(lockUI->boolValue());
}

