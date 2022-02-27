/*
  ==============================================================================

    VirtualFaderColGrid.cpp
    Created: 19 Feb 2022 12:19:42am
    Author:  No

  ==============================================================================
*/

#include <JuceHeader.h>
#include "VirtualFaderColGrid.h"
#include "UserInputManager.h"
#include "BKEngine.h"
#include "VirtualFaderCol.h"
#include "VirtualFaderColManager.h"

//==============================================================================
VirtualFaderColGridUI::VirtualFaderColGridUI(const String& contentName):
    ShapeShifterContent(VirtualFaderColGrid::getInstance(), contentName)
{
    
}

VirtualFaderColGridUI::~VirtualFaderColGridUI()
{
}

juce_ImplementSingleton(VirtualFaderColGrid);

VirtualFaderColGrid::VirtualFaderColGrid():
    BaseItem("VirtualFaderColGrid")
{
    addMouseListener(this, true);
    // addAndMakeVisible(scroll);
    // scroll.addListener(this);

    addAndMakeVisible(pagePlusBtn);
    pagePlusBtn.addListener(this);
    pagePlusBtn.setButtonText("Page +");

    addAndMakeVisible(pageMinusBtn);
    pageMinusBtn.addListener(this);
    pageMinusBtn.setButtonText("Page -");

    addAndMakeVisible(pageDisplayBtn);
    pageDisplayBtn.addListener(this);
    pageDisplayBtn.setEnabled(false);
    pageDisplayBtn.setButtonText("Page 1");

    initCells();


    // addAndMakeVisible(test);
    // test.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
}

VirtualFaderColGrid::~VirtualFaderColGrid()
{
}

void VirtualFaderColGrid::paint (juce::Graphics& g)
{
}

void VirtualFaderColGrid::resized()
{
    int h = getHeight();
    int w = getWidth();

    int btnWidth = w/cols;
    int nElements = 1;
    nElements += nRotaries;
    nElements += nAbove;
    nElements += sizeFader;
    nElements += nBelow;

    int btnHeight = h/(nElements);

    int staticBtnWith = btnWidth;
    if (cols < 3) {
        staticBtnWith = w/3;
    }
    pageMinusBtn.setBounds(w - (3 * staticBtnWith), 0, staticBtnWith, btnHeight);
    pageDisplayBtn.setBounds(w - (2 * staticBtnWith), 0, staticBtnWith, btnHeight);
    pagePlusBtn.setBounds(w - (1 * staticBtnWith), 0, staticBtnWith, btnHeight);

    for (int x = 0; x < cols; x++) {
        int coordX = x*btnWidth;
        int currentCell = 1;
        for (int i = 0; i < nRotaries; i++) {
            rotaries[x]->getRawDataPointer()[i]->setBounds(coordX, currentCell * btnHeight, btnWidth, btnHeight);
            currentCell += 1;
        }
        for (int i = 0; i < nAbove; i++) {
            buttons[x]->getRawDataPointer()[i]->setBounds(coordX, currentCell * btnHeight, btnWidth, btnHeight);
            currentCell += 1;
        }
        faders[x]->setBounds(coordX, currentCell * btnHeight, btnWidth, btnHeight*sizeFader);
        currentCell+= sizeFader;
        for (int i = 0; i < nBelow; i++) {
            buttons[x]->getRawDataPointer()[i + nAbove]->setBounds(coordX, currentCell * btnHeight, btnWidth, btnHeight);
            currentCell += 1;
        }

    }

}

void VirtualFaderColGrid::initCells() {
    if (engine == nullptr ) { return; }
    // gridButtons.clear();
    buttons.clear();
    rotaries.clear();
    faders.clear();
    cols = engine->virtualFaderCols->getValue();
    nRotaries = engine->virtualFaderRotary->getValue();
    nAbove = engine->virtualFaderAbove->getValue();
    sizeFader = engine->virtualFaderSize->getValue();
    nBelow = engine->virtualFaderBelow->getValue();

    for (int x = 0; x < cols; x++) {
        buttons.add(new OwnedArray<TextButton>());
        rotaries.add(new OwnedArray<Slider>());
        
        for (int i = 0; i < nRotaries; i++) {
            Slider* s = new Slider();
            addAndMakeVisible(s);
            s->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
            s->setColour(Slider::rotarySliderFillColourId, Colour(127, 127, 127));
            s->setRange(0, 1);
            s->setValue(0, juce::dontSendNotification);
            s->addListener(this);
            s->setTextBoxStyle(Slider::NoTextBox,true,0,0);
            rotaries[x]->add(s);
        }

        for (int i = 0; i < nAbove; i++) {
            TextButton* b = new TextButton();
            addAndMakeVisible(b);
            b->addListener(this);
            buttons[x]->add(b);
        }

        Slider* f = new Slider();
        addAndMakeVisible(f);
        f->setColour(Slider::thumbColourId, Colour(127, 127, 127));
        f->setSliderStyle(Slider::LinearBarVertical);
        f->setRange(0, 1);
        f->setValue(0, juce::dontSendNotification);
        f->addListener(this);
        f->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
        faders.add(f);

        for (int i = 0; i < nBelow; i++) {
            TextButton* b = new TextButton();
            addAndMakeVisible(b);
            b->addListener(this);
            buttons[x]->add(b);
        }

    }

    fillCells();
    resized();
}

void VirtualFaderColGrid::fillCells() {
//    buttonToVirtualFaderCol.clear();
    //pageDisplayBtn.setButtonText("Page "+String(page));
    //for (int i = 0; i < gridButtons.size(); i++) {
    //    gridButtons[i]->setColour(TextButton::buttonColourId, Colour(63,63,63));
    //    gridButtons[i]->setButtonText("");
    //}
    //for (int i = 0; i < VirtualFaderColManager::getInstance()->items.size(); i++) {
    //    VirtualFaderCol* vb = VirtualFaderColManager::getInstance()->items[i];
    //    if ((int)vb->pageNumber->getValue() == page) {
    //        int r = vb->rowNumber->getValue();
    //        int c = vb->colNumber->getValue();
    //        String btnText = vb->getBtnText();
    //        if (r != 0 && c != 0 && r < rows && c < cols) {
    //            int index = ((r-1)*cols)+(c-1);
    //            gridButtons[index]->setColour(TextButton::buttonColourId, Colour(127, 127, 127));
    //            gridButtons[index]->setButtonText(btnText);
    //            buttonToVirtualFaderCol.set(gridButtons[index], vb);
    //        }
    //    }
    //}
}

void VirtualFaderColGrid::buttonClicked(juce::Button* button) {
    if ((TextButton*)button == &pagePlusBtn) {
        page += 1;
        fillCells();
    } else if ((TextButton*)button == &pageMinusBtn) {
        page = jmax(1, page-1);
        fillCells();
    }
    else {
        buttonPressedUp((TextButton*)button);
    }
}

void VirtualFaderColGrid::buttonStateChanged(juce::Button* button) {
    if (button->isDown()) {
        buttonPressedDown((TextButton*)button);
    } else if (button->isMouseButtonDown()) {
        buttonPressedUp((TextButton*)button);
    }
}

void VirtualFaderColGrid::sliderValueChanged(Slider* slider) {

}

void VirtualFaderColGrid::buttonPressedDown(TextButton* t) {
    //int index = gridButtons.indexOf(t) + 1;
    //// UserInputManager::getInstance()->gridViewCellPressed("VirtualFaderCol", index);
    //Programmer* p = UserInputManager::getInstance()->getProgrammer();
    //if (p->cliActionType->getValue() != "") {
    //    if (p->userCanPressTargetType) {
    //        p->processUserInput("VirtualFaderCol");
    //        p->processUserInput(String(index));
    //        if (p->userCanPressGo) {
    //            p->processUserInput("enter");
    //        }
    //    }
    //}
    //else {
    //    VirtualFaderCol * vb = buttonToVirtualFaderCol.getReference(t);
    //    if (vb != nullptr) {
    //        vb -> pressed();
    //    }
    //}
}

void VirtualFaderColGrid::buttonPressedUp(TextButton* t) {
   //VirtualFaderCol* vb = buttonToVirtualFaderCol.getReference(t);
   //if (vb != nullptr) {
   //    vb->released();
   //}
}

void VirtualFaderColGrid::editCell(int id) {
    //id = id - 1;
    //if (id >= 0 && id >= gridButtons.size()) {return; }
    //TextButton* b = gridButtons[id];
    //VirtualFaderCol* vb = buttonToVirtualFaderCol.getReference(b);
    //if (vb == nullptr) {
    //    vb = VirtualFaderColManager::getInstance()->addItem();
    //    vb->pageNumber->setValue(page);
    //    vb->rowNumber->setValue(1+(id / cols));
    //    vb->colNumber->setValue(1+(id % cols));
    //    fillCells();
    //}
    //vb->selectThis();
}

void VirtualFaderColGrid::deleteCell(int id) {
    //id = id-1;
    //if (id >= 0 && id >= gridButtons.size()) { return; }
    //TextButton* b = gridButtons[id];
    //VirtualFaderCol* vb = buttonToVirtualFaderCol.getReference(b);
    //if (vb != nullptr) {
    //    VirtualFaderColManager::getInstance()->removeItem(vb);
    //    fillCells();
    //}
}

void VirtualFaderColGrid::moveCell(int idFrom, int idTo) {
    //idFrom = idFrom - 1;
    //idTo = idTo - 1;
    //if (idFrom >= 0 && idFrom >= gridButtons.size()) { return; }
    //if (idTo>= 0 && idTo >= gridButtons.size()) { return; }
    //TextButton* b = gridButtons[idFrom];
    //VirtualFaderCol* vb = buttonToVirtualFaderCol.getReference(b);
    //if (vb != nullptr) {
    //    vb->rowNumber->setValue(1+(idTo / cols));
    //    vb->colNumber->setValue(1+(idTo % cols));
    //    fillCells();
    //}
}

void VirtualFaderColGrid::copyCell(int idFrom, int idTo) {
    //idFrom = idFrom - 1;
    //idTo = idTo - 1;
    //if (idFrom >= 0 && idFrom >= gridButtons.size()) { return; }
    //if (idTo>= 0 && idTo >= gridButtons.size()) { return; }
    //TextButton* b = gridButtons[idFrom];
    //VirtualFaderCol* vb = buttonToVirtualFaderCol.getReference(b);
    //if (vb != nullptr) {
    //    VirtualFaderCol* vbCopy = nullptr;
    //    vbCopy = VirtualFaderColManager::getInstance()->addItemFromData(vb->getJSONData());
    //    vbCopy->rowNumber->setValue(1+(idTo / cols));
    //    vbCopy->colNumber->setValue(1+(idTo % cols));
    //    fillCells();
    //    vbCopy->selectThis();
    //}
}

