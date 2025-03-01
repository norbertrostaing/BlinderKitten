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
    reconstructLibraries();
}

void VirtualFaderColManager::removeItemInternal(VirtualFaderCol* o)
{
    reconstructLibraries();
}

void VirtualFaderColManager::onContainerParameterChanged(Parameter* p)
{
}

void VirtualFaderColManager::reconstructLibraries()
{
    usingLibraries.enter();
    buttonLibrary.clear();
    sliderLibrary.clear();
    for (int i = 0; i < items.size(); i++) {
        VirtualFaderCol* vfc = items[i];
        String page = vfc->pageNumber->stringValue();
        String col = vfc->colNumber->stringValue();

        for (int n = 0; n < vfc->aboveButtons.items.size(); n++) {
            String row = String(n + 1);
            String address = "above/" + page + "/" + col + "/" + row;
            buttonLibrary.set(address, vfc->aboveButtons.items[n]);
        }
        for (int n = 0; n < vfc->belowButtons.items.size(); n++) {
            String row = String(n + 1);
            String address = "below/" + page + "/" + col + "/" + row;
            buttonLibrary.set(address, vfc->belowButtons.items[n]);
        }
        for (int n = 0; n < vfc->rotaries.items.size(); n++) {
            String row = String(n + 1);
            String address = "rotary/" + page + "/" + col + "/" + row;
            sliderLibrary.set(address, vfc->rotaries.items[n]);
        }
        String address = "fader/" + page + "/" + col;
        sliderLibrary.set(address, &vfc->fader);

    }
    usingLibraries.exit();
}


void VirtualFaderColManager::setAboveButtonValue(int page, int col, int row, float value, String origin)
{
    VirtualFaderButton* vb = nullptr;
    String address = "above/" + String(page) + "/" + String(col) + "/" + String(row);
    String addressGen = "above/0/" + String(col) + "/" + String(row);
    usingLibraries.enter();
    for (auto it = buttonLibrary.begin(); it != buttonLibrary.end() && vb == nullptr; it.next()) {
        if (it.getKey() == address || it.getKey() == addressGen) {
            vb = it.getValue();
        }
    }
    usingLibraries.exit();
    if (vb != nullptr) {
        if (value == 1 ) {
            vb->pressed();
        }
        else {
            vb->released();
        }
    }
}

void VirtualFaderColManager::setBelowButtonValue(int page, int col, int row, float value, String origin)
{
    VirtualFaderButton* vb = nullptr;
    String address = "below/" + String(page) + "/" + String(col) + "/" + String(row);
    String addressGen = "below/0/" + String(col) + "/" + String(row);
    usingLibraries.enter();
    for (auto it = buttonLibrary.begin(); it != buttonLibrary.end() && vb == nullptr; it.next()) {
        if (it.getKey() == address || it.getKey() == addressGen) {
            vb = it.getValue();
        }
    }
    usingLibraries.exit();
    if (vb != nullptr) {
        if (value == 1) {
            vb->pressed();
        }
        else {
            vb->released();
        }
    }
}

void VirtualFaderColManager::setRotaryValue(int page, int col, int row, float value, String origin, int incrementIndex, bool isRelative)
{
    VirtualFaderSlider* vs = nullptr;
    String address = "rotary/" + String(page) + "/" + String(col) + "/" + String(row);
    String addressGen = "rotary/0/" + String(col) + "/" + String(row);
    usingLibraries.enter();
    for (auto it = sliderLibrary.begin(); it != sliderLibrary.end() && vs == nullptr; it.next()) {
        if (it.getKey() == address || it.getKey() == addressGen) {
            vs = it.getValue();
        }
    }
    usingLibraries.exit();
    if (vs != nullptr) {
        vs->moved(value, origin, incrementIndex, isRelative);
    }
}

void VirtualFaderColManager::setFaderValue(int page, int col, float value, String origin, int incrementIndex, bool isRelative)
{
    VirtualFaderSlider* vs = nullptr;
    String address = "fader/" + String(page) + "/" + String(col);
    String addressGen = "fader/0/" + String(col);
    usingLibraries.enter();
    for (auto it = sliderLibrary.begin(); it != sliderLibrary.end() && vs == nullptr; it.next()) {
        if (it.getKey() == address || it.getKey() == addressGen) {
            vs = it.getValue();
        }
    }
    usingLibraries.exit();
    if (vs != nullptr) {
        vs->moved(value, origin, incrementIndex, isRelative);
    }
}

