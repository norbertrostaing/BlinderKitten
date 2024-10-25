#include "Layout.h"
#include "LayoutManager.h"
/*
  ==============================================================================

    ObjectManager.cpp
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

juce_ImplementSingleton(LayoutManager);

int compare(Layout* A, Layout* B) {
    return (int)A->id->getValue() - (int)B->id->getValue();
}

LayoutManager::LayoutManager() :
    BaseManager("2D Plans")    
{
    itemDataType = "Layout";
    selectItemWhenCreated = true;
    comparator.compareFunc = compare;
    autoReorderOnAdd = true;
}

LayoutManager::~LayoutManager()
{
}


void LayoutManager::onContainerParameterChanged(Parameter* p)
{
   // if (p == lockUI) for (auto& i : items) i->isUILocked->setValue(lockUI->boolValue());
}

void LayoutManager::importData(var data)
{
    Array<var>* elements = data.getArray();
    if (elements == nullptr) {return;}
    for (var d : *elements) {
        String name = d.getProperty("niceName", "nop").toString().trim();
        Layout* cf = nullptr;
        for (int i = 0; i < items.size(); i++) {
            if (items[i]->niceName.trim() == name) {
                cf = items[i];
            }
        }

        if (cf == nullptr) {
            var temp = var();
            temp.append(d);
            addItemsFromData(temp);
        }
        else {
            Array<var>* childItems = d.getProperty("containers", var()).getProperty("BKPaths", var()).getProperty("items", var()).getArray();
            for (var ct : *childItems) {
                String paramName = ct.getProperty("niceName", "plop").toString();
                bool alreadyHere = false;
                for (int i = 0; i < cf->paths.items.size(); i++) {
                    if (cf->paths.items[i]->niceName == paramName) {
                        alreadyHere = true;
                    }
                }
                if (!alreadyHere) {
                    cf->paths.addItemFromData(ct);
                }
            }
        }
    }
}

void LayoutManager::updateOrderedElements()
{
    orderedElements.clear();
    for (int iFam = 0; iFam < items.size(); iFam++) {
        for (int iType = 0; iType < items[iFam]->paths.items.size(); iType++) {
            orderedElements.add(items[iFam]->paths.items[iType]);
        }
    }
}

