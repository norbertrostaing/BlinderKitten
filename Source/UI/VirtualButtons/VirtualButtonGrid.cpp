/*
  ==============================================================================

    VirtualButtonGrid.cpp
    Created: 19 Feb 2022 12:19:42am
    Author:  No

  ==============================================================================
*/

#include <JuceHeader.h>
#include "VirtualButtonGrid.h"
#include "UserInputManager.h"
#include "BKEngine.h"
#include "VirtualButton.h"
#include "VirtualButtonManager.h"

//==============================================================================
VirtualButtonGridUI::VirtualButtonGridUI(const String& contentName):
    ShapeShifterContent(VirtualButtonGrid::getInstance(), contentName)
{
    
}

VirtualButtonGridUI::~VirtualButtonGridUI()
{
}

juce_ImplementSingleton(VirtualButtonGrid);

VirtualButtonGrid::VirtualButtonGrid():
    BaseItem("VirtualButtonGrid")
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

VirtualButtonGrid::~VirtualButtonGrid()
{
    buttonToVirtualButton.clear();
}

void VirtualButtonGrid::paint (juce::Graphics& g)
{
}

void VirtualButtonGrid::resized()
{
    int h = getHeight();
    int w = getWidth();

    int btnWidth = w/cols;
    int btnHeight = h/(rows+1);

    int staticBtnWith = btnWidth;
    if (cols < 3) {
        staticBtnWith = w/3;
    }
    pageMinusBtn.setBounds(w - (3 * staticBtnWith), 0, staticBtnWith, btnHeight);
    pageDisplayBtn.setBounds(w - (2 * staticBtnWith), 0, staticBtnWith, btnHeight);
    pagePlusBtn.setBounds(w - (1 * staticBtnWith), 0, staticBtnWith, btnHeight);

    for (int i = 0; i < gridButtons.size(); i++) {
        int x = i%cols;
        int y = (i/cols)+1;

        x *= btnWidth;
        y *= btnHeight;
        gridButtons[i]->setBounds(x,y, btnWidth, btnHeight);
    }
}

void VirtualButtonGrid::initCells() {
    if (engine == nullptr ) { return; }
    gridButtons.clear();
    cols = engine->virtualButtonGridCols->getValue();
    rows = engine->virtualButtonGridRows->getValue();
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            TextButton* t = new TextButton();
            t->setButtonText(String(x+1)+"x"+ String(y + 1));
            t->addListener(this);
            addAndMakeVisible(t);
            gridButtons.add(t);
        }
    }
    fillCells();
    resized();
}

void VirtualButtonGrid::fillCells() {
    buttonToVirtualButton.clear();
    pageDisplayBtn.setButtonText("Page "+String(page));
    for (int i = 0; i < gridButtons.size(); i++) {
        gridButtons[i]->setColour(TextButton::buttonColourId, Colour(63,63,63));
        gridButtons[i]->setButtonText("");
    }
    for (int i = 0; i < VirtualButtonManager::getInstance()->items.size(); i++) {
        VirtualButton* vb = VirtualButtonManager::getInstance()->items[i];
        if ((int)vb->pageNumber->getValue() == page) {
            int r = vb->rowNumber->getValue();
            int c = vb->colNumber->getValue();
            String btnText = vb->getBtnText();
            if (r != 0 && c != 0 && r < rows && c < cols) {
                int index = ((r-1)*cols)+(c-1);
                gridButtons[index]->setColour(TextButton::buttonColourId, Colour(127, 127, 127));
                gridButtons[index]->setButtonText(btnText);
                buttonToVirtualButton.set(gridButtons[index], vb);
            }
        }
    }
}

void VirtualButtonGrid::buttonClicked(juce::Button* button) {
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

void VirtualButtonGrid::buttonStateChanged(juce::Button* button) {
    if (button->isDown()) {
        buttonPressedDown((TextButton*)button);
    } else if (button->isMouseButtonDown()) {
        buttonPressedUp((TextButton*)button);
    }
}

void VirtualButtonGrid::buttonPressedDown(TextButton* t) {
    int index = gridButtons.indexOf(t) + 1;
    // UserInputManager::getInstance()->gridViewCellPressed("virtualbutton", index);
    Programmer* p = UserInputManager::getInstance()->getProgrammer();
    if (p->cliActionType->getValue() != "") {
        if (p->userCanPressTargetType) {
            p->processUserInput("virtualbutton");
            p->processUserInput(String(index));
            if (p->userCanPressGo) {
                p->processUserInput("enter");
            }
        }
    }
    else {
        VirtualButton * vb = buttonToVirtualButton.getReference(t);
        if (vb != nullptr) {
            vb -> pressed();
        }
    }
}

void VirtualButtonGrid::buttonPressedUp(TextButton* t) {
   VirtualButton* vb = buttonToVirtualButton.getReference(t);
   if (vb != nullptr) {
       vb->released();
   }
}

void VirtualButtonGrid::editCell(int id) {
    id = id - 1;
    if (id >= 0 && id >= gridButtons.size()) {return; }
    TextButton* b = gridButtons[id];
    VirtualButton* vb = buttonToVirtualButton.getReference(b);
    if (vb == nullptr) {
        vb = VirtualButtonManager::getInstance()->addItem();
        vb->pageNumber->setValue(page);
        vb->rowNumber->setValue(1+(id / cols));
        vb->colNumber->setValue(1+(id % cols));
        fillCells();
    }
    vb->selectThis();
}

void VirtualButtonGrid::deleteCell(int id) {
    id = id-1;
    if (id >= 0 && id >= gridButtons.size()) { return; }
    TextButton* b = gridButtons[id];
    VirtualButton* vb = buttonToVirtualButton.getReference(b);
    if (vb != nullptr) {
        VirtualButtonManager::getInstance()->removeItem(vb);
        fillCells();
    }
}

void VirtualButtonGrid::moveCell(int idFrom, int idTo) {
    idFrom = idFrom - 1;
    idTo = idTo - 1;
    if (idFrom >= 0 && idFrom >= gridButtons.size()) { return; }
    if (idTo>= 0 && idTo >= gridButtons.size()) { return; }
    TextButton* b = gridButtons[idFrom];
    VirtualButton* vb = buttonToVirtualButton.getReference(b);
    if (vb != nullptr) {
        vb->rowNumber->setValue(1+(idTo / cols));
        vb->colNumber->setValue(1+(idTo % cols));
        fillCells();
    }
}

void VirtualButtonGrid::copyCell(int idFrom, int idTo) {
    idFrom = idFrom - 1;
    idTo = idTo - 1;
    if (idFrom >= 0 && idFrom >= gridButtons.size()) { return; }
    if (idTo>= 0 && idTo >= gridButtons.size()) { return; }
    TextButton* b = gridButtons[idFrom];
    VirtualButton* vb = buttonToVirtualButton.getReference(b);
    if (vb != nullptr) {
        VirtualButton* vbCopy = nullptr;
        vbCopy = VirtualButtonManager::getInstance()->addItemFromData(vb->getJSONData());
        vbCopy->rowNumber->setValue(1+(idTo / cols));
        vbCopy->colNumber->setValue(1+(idTo % cols));
        fillCells();
        vbCopy->selectThis();
    }
}

VirtualButton* VirtualButtonGrid::getVirtualButton(int id, bool create)
{
    id = id - 1;
    if (id >= 0 && id >= gridButtons.size()) { return nullptr; }
    TextButton* b = gridButtons[id];
    VirtualButton* vb = buttonToVirtualButton.getReference(b);
    if (create) {
        vb = VirtualButtonManager::getInstance()->addItem();
        vb->pageNumber->setValue(page);
        vb->rowNumber->setValue(1 + (id / cols));
        vb->colNumber->setValue(1 + (id % cols));
    }
    return vb;
}

