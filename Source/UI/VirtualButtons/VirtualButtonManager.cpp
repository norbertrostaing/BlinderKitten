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
    reconstructLibrary();
}

void VirtualButtonManager::removeItemInternal(VirtualButton* o)
{
    reconstructLibrary();
}

void VirtualButtonManager::onContainerParameterChanged(Parameter* p)
{
}

void VirtualButtonManager::reconstructLibrary()
{
    usingLibrary.enter();
    library.clear();
    for (int i = 0; i < items.size(); i++) {
        String address = items[i]->pageNumber->stringValue() + "/"+ items[i]->colNumber->stringValue() + "/"+ items[i]->rowNumber->stringValue();
        library.set(address, items[i]);
    }
    usingLibrary.exit();
}

void VirtualButtonManager::setButtonValue(int page, int col, int row, float value, String origin)
{
    VirtualButton* vb = nullptr;
    String address = String(page) + "/" + String(col) + "/" + String(row);
    String addressGen =  + "0/" + String(col) + "/" + String(row);
    usingLibrary.enter();
    for (auto it = library.begin(); it != library.end() && vb == nullptr; it.next()) {
        if (it.getKey() == address || it.getKey() == addressGen) {
            vb = it.getValue();
        }
    }
    usingLibrary.exit();
    if (vb != nullptr) {
        if (value > 0) {
            vb->pressed();
        }
        else {
            vb->released();
        }
    }
}

