#include "MultiplicatorLink.h"
#include "MultiplicatorLinkManager.h"
#include "Multiplicator.h"
/*
  ==============================================================================

    ObjectManager.cpp
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

MultiplicatorLinkManager::MultiplicatorLinkManager(String name) :
    BaseManager(name)    {
    itemDataType = "MultiplicatorLink";
    selectItemWhenCreated = false;
       
}

MultiplicatorLinkManager::~MultiplicatorLinkManager()
{
}


void MultiplicatorLinkManager::addItemInternal(MultiplicatorLink* o, var data)
{
    // o->addMultiplicatorLinkListener(this);
    // if (!isCurrentlyLoadingData) o->globalID->setValue(getFirstAvailableObjectID(o));
}

void MultiplicatorLinkManager::removeItemInternal(MultiplicatorLink* o)
{
    // o->removeObjectListener(this);
}


void MultiplicatorLinkManager::onContainerParameterChanged(Parameter* p)
{
   // if (p == lockUI) for (auto& i : items) i->isUILocked->setValue(lockUI->boolValue());
}

float MultiplicatorLinkManager::getValue() {
    float val = 1;
    for (int i = 0; i < items.size(); i++) {
        if (items[i]->enabled->boolValue()) {

            Multiplicator* m = dynamic_cast<Multiplicator*>(items[i]->targetMult->targetContainer.get());
            if (m != nullptr) {
                val = val * (float)m->multValue->getValue();
            }
        }
    }
    return val;
}