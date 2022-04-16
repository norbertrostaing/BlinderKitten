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
#include "VirtualFaderButton.h"
#include "VirtualFaderSlider.h"

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

    int btnHeight = (h-20)/(nElements);

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
        columnLabels[x]->setBounds(coordX, h-20, btnWidth, 20);
        for (int i = 0; i < nRotaries; i++) {
            rotaries[x]->getRawDataPointer()[i]->setBounds(coordX, currentCell * btnHeight, btnWidth, btnHeight);
            rotaryLabels[x]->getRawDataPointer()[i]->setBounds(coordX, (currentCell+0.25) * btnHeight, btnWidth, btnHeight*0.25);
            currentCell += 1;
        }
        for (int i = 0; i < nAbove; i++) {
            aboveButtons[x]->getRawDataPointer()[i]->setBounds(coordX, currentCell * btnHeight, btnWidth, btnHeight);
            currentCell += 1;
        }
        faders[x]->setBounds(coordX, (currentCell * btnHeight), btnWidth, (btnHeight * sizeFader)-40);
        currentCell += sizeFader;
        faderLabels[x]->setBounds(coordX, (currentCell * btnHeight)-40, btnWidth, 40);
        for (int i = 0; i < nBelow; i++) {
            belowButtons[x]->getRawDataPointer()[i]->setBounds(coordX, currentCell * btnHeight, btnWidth, btnHeight);
            currentCell += 1;
        }

    }

}

void VirtualFaderColGrid::initCells() {
    if (engine == nullptr ) { return; }
    // gridButtons.clear();
    aboveButtons.clear();
    belowButtons.clear();
    rotaries.clear();
    faders.clear();
    faderLabels.clear();
    rotaryLabels.clear();
    columnLabels.clear();

    buttonToVFB.clear();
    sliderToVFS.clear();
    buttonColumnIndex.clear();
    sliderColumnIndex.clear();

    cols = engine->virtualFaderCols->getValue();
    nRotaries = engine->virtualFaderRotary->getValue();
    nAbove = engine->virtualFaderAbove->getValue();
    sizeFader = engine->virtualFaderSize->getValue();
    nBelow = engine->virtualFaderBelow->getValue();

    for (int x = 0; x < cols; x++) {
        aboveButtons.add(new OwnedArray<TextButton>());
        belowButtons.add(new OwnedArray<TextButton>());
        rotaries.add(new OwnedArray<Slider>());
        rotaryLabels.add(new OwnedArray<Label>());
        
        Label* mainLabel = new Label();
        addAndMakeVisible(mainLabel);
        mainLabel->setEditable(false);
        mainLabel->setText("", juce::dontSendNotification);
        mainLabel->setJustificationType(Justification(36));
        columnLabels.add(mainLabel);

        for (int i = 0; i < nRotaries; i++) {
            Slider* s = new Slider();
            addAndMakeVisible(s);
            s->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
            s->setRange(0, 1);
            s->setValue(0, juce::dontSendNotification);
            s->addListener(this);
            s->setTextBoxStyle(Slider::NoTextBox,true,0,0);
            rotaries[x]->add(s);
            sliderColumnIndex.set(s, x+1);
            Label * l = new Label();
            addAndMakeVisible(l);
            l->setEditable(false);
            l->setText("", juce::dontSendNotification);
            l->setJustificationType(Justification(36));
            rotaryLabels[x]->add(l);

        }

        for (int i = 0; i < nAbove; i++) {
            TextButton* b = new TextButton();
            addAndMakeVisible(b);
            b->addListener(this);
            aboveButtons[x]->add(b);
            buttonColumnIndex.set(b, x+1);
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
        sliderColumnIndex.set(f, x+1);
        Label* l = new Label();
        addAndMakeVisible(l);
        l->setEditable(false);
        l->setText("", juce::dontSendNotification);
        l->setJustificationType(Justification(36));

        faderLabels.add(l);

        for (int i = 0; i < nBelow; i++) {
            TextButton* b = new TextButton();
            addAndMakeVisible(b);
            b->addListener(this);
            belowButtons[x]->add(b);
            buttonColumnIndex.set(b, x+1);
        }
    }

    fillCells();
    resized();
}

void VirtualFaderColGrid::fillCells() {

    buttonToVFB.clear();
    sliderToVFS.clear();
    columnToVFC.clear();
    pageDisplayBtn.setButtonText("Page "+String(page));
    for (int i = 0; i < faders.size(); i++) {
        faders[i]->setColour(Slider::backgroundColourId, Colour(63, 63, 63));
        faders[i]->setColour(Slider::thumbColourId, Colour(63, 63, 63));
        faders[i]->setColour(Slider::trackColourId, Colour(63, 63, 63));
        faderLabels[i]->setText("", juce::dontSendNotification);
    }
    for (int i = 0; i < columnLabels.size(); i++) {
        columnLabels[i]->setText("", juce::dontSendNotification);
    }
    for (int i = 0; i < rotaries.size(); i++) {
        for (int j = 0; j < rotaries[i]->size(); j++) {
            rotaries[i]->getRawDataPointer()[j]->setColour(Slider::rotarySliderFillColourId, Colour(63, 63, 63));
            rotaryLabels[i]->getRawDataPointer()[j]->setText("", juce::dontSendNotification);
        }
    }
    for (int i = 0; i < aboveButtons.size(); i++) {
        for (int j = 0; j < aboveButtons[i]->size(); j++) {
            aboveButtons[i]->getRawDataPointer()[j]->setColour(TextButton::buttonColourId, Colour(63, 63, 63));
            aboveButtons[i]->getRawDataPointer()[j]->setButtonText("");
        }
    }
    for (int i = 0; i < belowButtons.size(); i++) {
        for (int j = 0; j < belowButtons[i]->size(); j++) {
            belowButtons[i]->getRawDataPointer()[j]->setColour(TextButton::buttonColourId, Colour(63, 63, 63));
            belowButtons[i]->getRawDataPointer()[j]->setButtonText("");
        }
    }

    for (int i = 0; i < VirtualFaderColManager::getInstance()->items.size(); i++) {
        VirtualFaderCol* vf = VirtualFaderColManager::getInstance()->items[i];
        if ((int)vf->pageNumber->getValue() == page) {
            int c = vf->colNumber->getValue();
            columnToVFC.set(c, vf);
            c = c - 1;
            String targName = vf->getTargetName();
            String targType = vf->getTargetType();
            if (c>=0 && c < cols) {
                columnLabels[c]->setText(targName, juce::dontSendNotification);
                for (int n = 0; n < vf->rotaries.items.size() && n < nRotaries; n++) {
                    VirtualFaderSlider* vs = vf->rotaries.items[n];
                    sliderToVFS.set(rotaries[c]->getRawDataPointer()[n], vs);
                    String text = vs->getBtnText(targType);
                    if (text != "") {
                        rotaryLabels[c]->getRawDataPointer()[n]->setText(text, juce::dontSendNotification);
                        rotaries[c]->getRawDataPointer()[n]->setColour(Slider::rotarySliderFillColourId, Colour(63, 63, 63));

                    }
                }
                for (int n = 0; n < vf->aboveButtons.items.size() && n < nAbove; n++) {
                    VirtualFaderButton* vb = vf->aboveButtons.items[n];
                    buttonToVFB.set(aboveButtons[c]->getRawDataPointer()[n], vb);
                    String text = vb->getBtnText(targType);
                    if (text != "") {
                        aboveButtons[c]->getRawDataPointer()[n]->setButtonText(text);
                        aboveButtons[c]->getRawDataPointer()[n]->setColour(TextButton::buttonColourId, Colour(127, 127, 127));
                    }
                }
                String text = vf->fader.getBtnText(targType);
                sliderToVFS.set(faders[c], &vf->fader);
                if (text != "") {
                    faderLabels[c]->setText(text, juce::dontSendNotification);
                    faders[c]->setColour(Slider::trackColourId, Colour(127, 127, 127));
                }

                for (int n = 0; n < vf->belowButtons.items.size() && n < nBelow; n++) {
                    VirtualFaderButton* vb = vf->belowButtons.items[n];
                    buttonToVFB.set(belowButtons[c]->getRawDataPointer()[n], vb);
                    String text = vb->getBtnText(targType);
                    if (text != "") {
                        belowButtons[c]->getRawDataPointer()[n]->setButtonText(text);
                        belowButtons[c]->getRawDataPointer()[n]->setColour(TextButton::buttonColourId, Colour(127, 127, 127));
                    }
                }
                //faderLabels[c]->setText(btnText, juce::dontSendNotification);
            }
        }
    }
    resized();
    updateSlidersValues();
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
    int col = sliderColumnIndex.getReference(slider);
    VirtualFaderCol* vfc = columnToVFC.getReference(col);
    if (vfc != nullptr) {
        VirtualFaderSlider* vf = sliderToVFS.getReference(slider);
        if (vf != nullptr) {
            vf->moved(slider->getValue(), vfc->targetType->getValue(), vfc->targetId->getValue());
        }
    }
}

void VirtualFaderColGrid::buttonPressedDown(TextButton* t) {
    int col = buttonColumnIndex.getReference(t);
    //// UserInputManager::getInstance()->gridViewCellPressed("VirtualFaderCol", index);
    const MessageManagerLock mmLock;
    Programmer* p = UserInputManager::getInstance()->getProgrammer();
    if (p->cliActionType->getValue() != "") {
        if (p->userCanPressTargetType) {
            p->processUserInput("VirtualFaderCol");
            p->processUserInput(String(col));
            if (p->userCanPressGo) {
                p->processUserInput("enter");
            }
        }
    }
    else {
        VirtualFaderCol * vfc = columnToVFC.getReference(col);
        if (vfc != nullptr) {
            VirtualFaderButton* vb = buttonToVFB.getReference(t);
            if (vb != nullptr) {
                vb->pressed(vfc->targetType->getValue(), vfc->targetId->getValue());
            }
        }
    }
}

void VirtualFaderColGrid::buttonPressedUp(TextButton* t) {
    int col = buttonColumnIndex.getReference(t);
    const MessageManagerLock mmLock;
    //// UserInputManager::getInstance()->gridViewCellPressed("VirtualFaderCol", index);
    Programmer* p = UserInputManager::getInstance()->getProgrammer();
    VirtualFaderCol* vfc = columnToVFC.getReference(col);
    if (vfc != nullptr) {
        VirtualFaderButton* vb = buttonToVFB.getReference(t);
        if (vb != nullptr) {
            vb->released(vfc->targetType->getValue(), vfc->targetId->getValue());
        }
    }
}

void VirtualFaderColGrid::editCell(int id) {
    if (id < 1 || id > cols) {return; }
    VirtualFaderCol* vf = columnToVFC.getReference(id);
    if (vf == nullptr) {
        vf = VirtualFaderColManager::getInstance()->addItem();
        vf->pageNumber->setValue(page);
        vf->colNumber->setValue(id);
        fillCells();
    }
    vf->selectThis();
}

void VirtualFaderColGrid::deleteCell(int id) {
    if (id < 1 || id > cols) { return; }
    VirtualFaderCol* vf = columnToVFC.getReference(id);
    if (vf != nullptr) {
        VirtualFaderColManager::getInstance()->removeItem(vf);
        fillCells();
    }
}

void VirtualFaderColGrid::moveCell(int idFrom, int idTo) {
    idFrom = idFrom;
    idTo = idTo;
    if (idFrom <= 0 || idFrom > cols) { return; }
    if (idTo <= 0 || idTo > cols) { return; }
    VirtualFaderCol* vf = columnToVFC.getReference(idFrom);
    if (vf != nullptr) {
        vf->colNumber->setValue(idTo);
        fillCells();
    }
}

void VirtualFaderColGrid::copyCell(int idFrom, int idTo) {
    idFrom = idFrom;
    idTo = idTo;
    if (idFrom <= 0 || idFrom > cols) { return; }
    if (idTo <= 0 || idTo > cols) { return; }
    VirtualFaderCol* vf = columnToVFC.getReference(idFrom);
    if (vf != nullptr) {
        VirtualFaderCol* vfCopy = nullptr;
        vfCopy = VirtualFaderColManager::getInstance()->addItemFromData(vf->getJSONData());
        vfCopy->colNumber->setValue(idTo);
        fillCells();
        vfCopy->selectThis();
    }
}

VirtualFaderCol* VirtualFaderColGrid::getVirtualFaderCol(int id, bool create)
{
    if (id < 1 || id > cols) { return nullptr; }
    VirtualFaderCol* vf = columnToVFC.getReference(id);
    if (create) {
        vf = VirtualFaderColManager::getInstance()->addItem();
        vf->pageNumber->setValue(page);
        vf->colNumber->setValue(id);
        vf->fader.targetType->setValueWithData("column");
        for (int i = 0; i < nRotaries; i++) {
            VirtualFaderSlider* temp = vf->rotaries.addItem();
            temp->targetType->setValueWithData("column");
        }
        for (int i = 0; i < nAbove; i++) {
            VirtualFaderButton* temp = vf->aboveButtons.addItem();
            temp->targetType->setValueWithData("column");
        }
        for (int i = 0; i < nBelow; i++) {
            VirtualFaderButton* temp = vf->belowButtons.addItem();
            temp->targetType->setValueWithData("column");
        }
    }
    return vf;
}

void VirtualFaderColGrid::updateSlidersValues()
{
    const MessageManagerLock mmLock;

    for (int i = 0; i < cols; i++) {
        VirtualFaderCol* vfc = columnToVFC.getReference(i+1);
        String colTargetType = "";
        int colTargetId = 0;

        if (vfc != nullptr) {
            colTargetType = vfc->getTargetType();
            colTargetId = vfc->targetId->getValue();
        }

        VirtualFaderSlider* s = sliderToVFS.getReference(faders[i]);
        float v = 0;
        if (s != nullptr) {
            v = s->getTargetValue(colTargetType, colTargetId);
        }
        faders[i]->setValue(v, juce::dontSendNotification);

        for (int r = 0; r < nRotaries; r++) {
            VirtualFaderSlider* s = sliderToVFS.getReference(rotaries[i]->getRawDataPointer()[r]);
            float v = 0;
            if (s != nullptr) {
                v = s->getTargetValue(colTargetType, colTargetId);
            }
            rotaries[i]->getRawDataPointer()[r]->setValue(v, juce::dontSendNotification);

        }
    }
}

