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
       
}

ChannelFamilyManager::~ChannelFamilyManager()
{
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

void ChannelFamilyManager::importData(var data)
{
    Array<var>* elements = data.getArray();

    for (var d : *elements) {
        String name = d.getProperty("niceName", "nop").toString();
        bool valid = false;
        ChannelFamily* cf = nullptr;
        for (int i = 0; i < items.size(); i++) {
            if (items[i]->niceName == name) {
                cf = items[i];
            }
        }

        if (cf == nullptr) {
            addItemsFromData(data);
        }
        else {
            Array<var>* childItems = d.getProperty("containers", var()).getProperty("channelTypes", var()).getProperty("items", var()).getArray();
            for (var ct : *childItems) {
                String paramName = ct.getProperty("niceName", "plop").toString();
                bool alreadyHere = false;
                for (int i = 0; i < cf->definitions.items.size(); i++) {
                    if (cf->definitions.items[i]->niceName == paramName) {
                        alreadyHere = true;
                    }
                }
                if (!alreadyHere) {
                    cf->definitions.addItemFromData(ct);
                }
                // LOG(ct.getProperty("niceName", "plop").toString());
            }
        }
    }
}

