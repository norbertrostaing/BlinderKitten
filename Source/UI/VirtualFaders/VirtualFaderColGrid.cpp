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
            //rotaryLabels[x]->getRawDataPointer()[i]->setBounds(coordX, (currentCell + 0.25) * btnHeight, btnWidth, btnHeight * 0.25);
            float fontHeight = rotaryLabels[x]->getRawDataPointer()[i]->getFont().getHeight();
            rotaryLabels[x]->getRawDataPointer()[i]->setBounds(coordX, (currentCell) * btnHeight, btnWidth, fontHeight);

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

void VirtualFaderColGrid::goToPage(int n) {
    page = jmax(1, n);
    resetFeedbacks();
    fillCells();
}

void VirtualFaderColGrid::resetFeedbacks()
{
    for (int c = 0; c < cols; c++) {
        String address0 = "/vfader/0/" + String(c+1);
        UserInputManager::getInstance()->feedback(address0, 0, "");
        for (int n = 0; n < nRotaries; n++) {
            address0 = "/vrotary/0/" + String(c+1) + "/" + String(n + 1);
            UserInputManager::getInstance()->feedback(address0, 0, "");
        }
        for (int n = 0; n < nAbove; n++) {
            address0 = "/vabovebutton/0/" + String(c + 1) + "/" + String(n + 1);
            UserInputManager::getInstance()->feedback(address0, 0, "");
        }
        for (int n = 0; n < nBelow; n++) {
            address0 = "/vbelowbutton/0/" + String(c + 1) + "/" + String(n + 1);
            UserInputManager::getInstance()->feedback(address0, 0, "");
        }
    }
}

void VirtualFaderColGrid::updateButtons()
{
    {
        for (int i = 0; i < VirtualFaderColManager::getInstance()->items.size(); i++) {
            for (int a = 0; a < VirtualFaderColManager::getInstance()->items[i]->aboveButtons.items.size(); a++) {
                VirtualFaderColManager::getInstance()->items[i]->aboveButtons.items[a]->updateStatus();
            }
            for (int b = 0; b < VirtualFaderColManager::getInstance()->items[i]->belowButtons.items.size(); b++) {
                VirtualFaderColManager::getInstance()->items[i]->belowButtons.items[b]->updateStatus();
            }
        }
        for (int c = 0; c < aboveButtons.size(); c++) {
            for (int a = 0; a < aboveButtons[c]->size(); a++) {
                auto btn = aboveButtons[c]->getRawDataPointer()[a];
                if (buttonToVFB.contains(btn) && buttonToVFB.getReference(btn) != nullptr) {
                    VirtualFaderButton* vb = buttonToVFB.getReference(btn);
                    if (vb->currentStatus == VirtualFaderButton::BTN_GENERIC) {
                        btn->setColour(TextButton::buttonColourId, juce::Colour(64, 64, 80));
                    }
                    else if (vb->currentStatus == VirtualFaderButton::BTN_ON) {
                        btn->setColour(TextButton::buttonColourId, juce::Colour(64, 80, 64));
                    }
                    else if (vb->currentStatus == VirtualFaderButton::BTN_OFF) {
                        btn->setColour(TextButton::buttonColourId, juce::Colour(64, 64, 64));
                    }
                    else if (vb->currentStatus == VirtualFaderButton::BTN_ON_LOADED) {
                        btn->setColour(TextButton::buttonColourId, juce::Colour(64, 80, 80));
                    }
                    else if (vb->currentStatus == VirtualFaderButton::BTN_OFF_LOADED) {
                        btn->setColour(TextButton::buttonColourId, juce::Colour(64, 80, 80));
                    }
                    else if (vb->currentStatus == VirtualFaderButton::BTN_CURRENTCUE) {
                        btn->setColour(TextButton::buttonColourId, juce::Colour(64, 120, 64));
                    }
                    else if (vb->currentStatus == VirtualFaderButton::BTN_LOADEDCUE) {
                        btn->setColour(TextButton::buttonColourId, juce::Colour(64, 64, 120));
                    }
                    else {
                        btn->setColour(TextButton::buttonColourId, juce::Colour(32, 32, 32));
                    }
                }
                else {
                    btn->setColour(TextButton::buttonColourId, juce::Colour(32, 32, 32));
                }
            }
        }
        for (int c = 0; c < belowButtons.size(); c++) {
            for (int a = 0; a < belowButtons[c]->size(); a++) {
                auto btn = belowButtons[c]->getRawDataPointer()[a];
                if (buttonToVFB.contains(btn) && buttonToVFB.getReference(btn) != nullptr) {
                    VirtualFaderButton* vb = buttonToVFB.getReference(btn);
                    if (vb->currentStatus == VirtualFaderButton::BTN_GENERIC) {
                        btn->setColour(TextButton::buttonColourId, juce::Colour(64, 64, 80));
                    }
                    else if (vb->currentStatus == VirtualFaderButton::BTN_ON) {
                        btn->setColour(TextButton::buttonColourId, juce::Colour(64, 80, 64));
                    }
                    else if (vb->currentStatus == VirtualFaderButton::BTN_OFF) {
                        btn->setColour(TextButton::buttonColourId, juce::Colour(64, 64, 64));
                    }
                    else if (vb->currentStatus == VirtualFaderButton::BTN_ON_LOADED) {
                        btn->setColour(TextButton::buttonColourId, juce::Colour(64, 80, 80));
                    }
                    else if (vb->currentStatus == VirtualFaderButton::BTN_OFF_LOADED) {
                        btn->setColour(TextButton::buttonColourId, juce::Colour(64, 80, 80));
                    }
                    else if (vb->currentStatus == VirtualFaderButton::BTN_CURRENTCUE) {
                        btn->setColour(TextButton::buttonColourId, juce::Colour(64, 120, 64));
                    }
                    else if (vb->currentStatus == VirtualFaderButton::BTN_LOADEDCUE) {
                        btn->setColour(TextButton::buttonColourId, juce::Colour(64, 64, 120));
                    }
                    else {
                        btn->setColour(TextButton::buttonColourId, juce::Colour(32, 32, 32));
                    }
                }
                else {
                    btn->setColour(TextButton::buttonColourId, juce::Colour(32, 32, 32));
                }
            }
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
    buttonToIndex.clear();
    indexToButton.clear();
    buttonIsAbove.clear();
    int currentIndex = 0;

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
        mainLabel->setMinimumHorizontalScale(1.);
        mainLabel->setEditable(false);
        mainLabel->setText("", juce::dontSendNotification);
        mainLabel->setJustificationType(juce::Justification::centredTop);
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
            sliderToIndex.set(s, currentIndex);
            indexToSlider.set(currentIndex, s);
            currentIndex++;
            Label * l = new Label();
            addAndMakeVisible(l);
            l->setMinimumHorizontalScale(1.);
            l->setEditable(false);
            l->setText("", juce::dontSendNotification);
            l->setJustificationType(juce::Justification::centredTop);
            rotaryLabels[x]->add(l);

        }

        for (int i = 0; i < nAbove; i++) {
            TextButton* b = new TextButton();
            addAndMakeVisible(b);
            b->addListener(this);
            aboveButtons[x]->add(b);
            buttonColumnIndex.set(b, x+1);
            buttonToIndex.set(b, currentIndex);
            indexToButton.set(currentIndex, b);
            buttonIsAbove.set(b, true);
            currentIndex++;
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
        sliderToIndex.set(f, currentIndex);
        indexToSlider.set(currentIndex, f);
        currentIndex++;
        Label* l = new Label();
        addAndMakeVisible(l);
        l->setMinimumHorizontalScale(1.);
        l->setEditable(false);
        l->setText("", juce::dontSendNotification);
        l->setJustificationType(juce::Justification::centredTop);

        faderLabels.add(l);

        for (int i = 0; i < nBelow; i++) {
            TextButton* b = new TextButton();
            addAndMakeVisible(b);
            b->addListener(this);
            belowButtons[x]->add(b);
            buttonColumnIndex.set(b, x+1);
            buttonToIndex.set(b, currentIndex);
            indexToButton.set(currentIndex, b);
            buttonIsAbove.set(b, false);
            currentIndex++;
        }
    }

    fillCells();
    resized();
}

void VirtualFaderColGrid::fillCells() {

    const MessageManagerLock mmLock;
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
            aboveButtons[i]->getRawDataPointer()[j]->setColour(TextButton::buttonColourId, Colour(40, 40, 40));
            aboveButtons[i]->getRawDataPointer()[j]->setButtonText("");
        }
    }
    for (int i = 0; i < belowButtons.size(); i++) {
        for (int j = 0; j < belowButtons[i]->size(); j++) {
            belowButtons[i]->getRawDataPointer()[j]->setColour(TextButton::buttonColourId, Colour(40, 40, 40));
            belowButtons[i]->getRawDataPointer()[j]->setButtonText("");
        }
    }

    for (int i = 0; i < VirtualFaderColManager::getInstance()->items.size(); i++) {
        VirtualFaderCol* vf = VirtualFaderColManager::getInstance()->items[i];
        if (vf->pageNumber->intValue() == page || vf->pageNumber->intValue() == 0) {
            int c = vf->colNumber->getValue();
            columnToVFC.set(c, vf);
            c = c - 1;
            String targName = vf->getTargetName();
            String targType = vf->getTargetType();
            if (c>=0 && c < cols) {
                columnLabels[c]->setText(targName, juce::dontSendNotification);
                for (int n = 0; n < vf->rotaries.items.size() && n < nRotaries; n++) {
                    if (!sliderToVFS.contains(rotaries[c]->getRawDataPointer()[n])) {
                        VirtualFaderSlider* vs = vf->rotaries.items[n];
                        sliderToVFS.set(rotaries[c]->getRawDataPointer()[n], vs);
                        String text = vs->getBtnText(targType);
                        if (text != "") {
                            rotaryLabels[c]->getRawDataPointer()[n]->setText(text, juce::dontSendNotification);
                            rotaries[c]->getRawDataPointer()[n]->setColour(Slider::rotarySliderFillColourId, Colour(63, 63, 63));

                        }
                    }
                }
                for (int n = 0; n < vf->aboveButtons.items.size() && n < nAbove; n++) {
                    if (!buttonToVFB.contains(aboveButtons[c]->getRawDataPointer()[n])) {
                        VirtualFaderButton* vb = vf->aboveButtons.items[n];
                        buttonToVFB.set(aboveButtons[c]->getRawDataPointer()[n], vb);
                        String text = vb->getBtnText(targType);
                        if (text != "") {
                            aboveButtons[c]->getRawDataPointer()[n]->setButtonText(text);
                            aboveButtons[c]->getRawDataPointer()[n]->removeColour(TextButton::buttonColourId);
                        }
                    }
                }
                if (!sliderToVFS.contains(faders[c])) {
                    sliderToVFS.set(faders[c], &vf->fader);
                    String text = vf->fader.getBtnText(targType);
                    if (text != "") {
                        faderLabels[c]->setText(text, juce::dontSendNotification);
                        faders[c]->setColour(Slider::trackColourId, Colour(127, 127, 127));
                    }
                }

                for (int n = 0; n < vf->belowButtons.items.size() && n < nBelow; n++) {
                    if (!buttonToVFB.contains(belowButtons[c]->getRawDataPointer()[n])) {
                        VirtualFaderButton* vb = vf->belowButtons.items[n];
                        buttonToVFB.set(belowButtons[c]->getRawDataPointer()[n], vb);
                        String btnText = vb->getBtnText(targType);
                        if (btnText != "") {
                            belowButtons[c]->getRawDataPointer()[n]->setButtonText(btnText);
                            belowButtons[c]->getRawDataPointer()[n]->removeColour(TextButton::buttonColourId);
                        }
                    }
                }
                //faderLabels[c]->setText(btnText, juce::dontSendNotification);
            }
        }
    }
    resized();
    updateSlidersValues();
    updateButtons();
}

void VirtualFaderColGrid::buttonClicked(juce::Button* button) {
    if ((TextButton*)button == &pagePlusBtn) {
        page += 1;
        resetFeedbacks();
        fillCells();
    } else if ((TextButton*)button == &pageMinusBtn) {
        page = jmax(1, page-1);
        resetFeedbacks();
        fillCells();
    }
    else {
        buttonPressedUp((TextButton*)button);
    }
    UserInputManager::getInstance()->resetFocus();
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

    sliderClicked(slider);

    VirtualFaderCol* vfc = columnToVFC.getReference(col);
    if (vfc != nullptr) {
        VirtualFaderSlider* vf = sliderToVFS.getReference(slider);
        if (vf != nullptr) {
            vf->moved(slider->getValue(), "", false);
        }
    }
}

void VirtualFaderColGrid::mouseDown(const MouseEvent& m)
{
    Component* elmt = m.eventComponent;
    Slider* s = dynamic_cast<Slider*>(elmt);
    if (s != nullptr) {
        sliderClicked(s);
    }
}

void VirtualFaderColGrid::buttonPressedDown(TextButton* t) {
    int col = buttonColumnIndex.getReference(t);
    //// UserInputManager::getInstance()->gridViewCellPressed("VirtualFaderCol", index);
    const MessageManagerLock mmLock;
    Programmer* p = UserInputManager::getInstance()->getProgrammer(true);
    if (p->cliActionType->getValue() != "") {
        if (p->userCanPressTargetType) {
            BKEngine* engine = dynamic_cast<BKEngine*>(Engine::mainEngine);
            String mode = engine->faderSelectionMode->getValueData();

            if (mode == "column") {
                p->processUserInput("VirtualFaderCol");
                p->processUserInput(String(col));
                if (p->userCanPressGo) {
                    p->processUserInput("enter");
                }
            }
            else {
                bool isAbove = buttonIsAbove.getReference(t);
                int index = buttonToIndex.getReference(t);
                p->processUserInput("virtualfaderelement");
                p->processUserInput(String(index));
                if (p->userCanPressGo) {
                    p->processUserInput("enter");
                }
            }
            
        }
    }
    else {
        VirtualFaderCol * vfc = columnToVFC.getReference(col);
        if (vfc != nullptr) {
            VirtualFaderButton* vb = buttonToVFB.getReference(t);
            if (vb != nullptr) {
                vb->pressed();
            }
        }
    }
}

void VirtualFaderColGrid::buttonPressedUp(TextButton* t) {
    int col = buttonColumnIndex.getReference(t);
    const MessageManagerLock mmLock;
    //// UserInputManager::getInstance()->gridViewCellPressed("VirtualFaderCol", index);
    //Programmer* p = UserInputManager::getInstance()->getProgrammer();
    VirtualFaderCol* vfc = columnToVFC.getReference(col);
    if (vfc != nullptr) {
        VirtualFaderButton* vb = buttonToVFB.getReference(t);
        if (vb != nullptr) {
            vb->released();
        }
    }
}

void VirtualFaderColGrid::sliderClicked(Slider* s)
{
    int col = sliderColumnIndex.getReference(s);

    Programmer* p = UserInputManager::getInstance()->getProgrammer(true);
    if (p->cliActionType->getValue() != "") {
        const MessageManagerLock mmLock;
        if (p->userCanPressTargetType) {
            BKEngine* engine = dynamic_cast<BKEngine*>(Engine::mainEngine);
            String mode = engine->faderSelectionMode->getValueData();

            if (mode == "column") {
                p->processUserInput("VirtualFaderCol");
                p->processUserInput(String(col));
                if (p->userCanPressGo) {
                    p->processUserInput("enter");
                }
            }
            else {
                int index = sliderToIndex.getReference(s);
                if (p->cliParamAType->getValueData().toString() != "virtualfaderelement" || p->cliParamAId->intValue() != index) {
                    p->processUserInput("virtualfaderelement");
                    p->processUserInput(String(index));
                    if (p->userCanPressGo) {
                        p->processUserInput("enter");
                    }
                }
            }

        }
    }
}

void VirtualFaderColGrid::editCol(int id) {
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

void VirtualFaderColGrid::deleteCol(int id) {
    if (id < 1 || id > cols) { return; }
    VirtualFaderCol* vf = columnToVFC.getReference(id);
    if (vf != nullptr) {
        VirtualFaderColManager::getInstance()->removeItem(vf);
        fillCells();
    }
}

void VirtualFaderColGrid::moveCol(int idFrom, int idTo) {
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

void VirtualFaderColGrid::copyCol(int idFrom, int idTo) {
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

void VirtualFaderColGrid::editElmt(int id)
{
    int col = id / (1 + nRotaries+nAbove+nBelow);
    editCol(col+1);
}

void VirtualFaderColGrid::deleteElmt(int id)
{
    if (indexToButton.contains(id) && indexToButton.getReference(id) != nullptr) {
        TextButton* b = indexToButton.getReference(id);
        if (buttonToVFB.contains(b)) {
            VirtualFaderButton* vfb = buttonToVFB.getReference(b);
            if (vfb != nullptr) {
                vfb->targetType->setValueWithData("disabled");
            }
        }
    }
    else if (indexToSlider.contains(id) && indexToSlider.getReference(id) != nullptr) {
        Slider* s = indexToSlider.getReference(id);
        if (sliderToVFS.contains(s)) {
            VirtualFaderSlider* vfs = sliderToVFS.getReference(s);
            if (vfs != nullptr) {
                vfs->targetType->setValueWithData("disabled");
            }
        }
    }
}

void VirtualFaderColGrid::moveElmt(int idFrom, int idTo)
{
    VirtualFaderButton* vfbFrom = getVirtualFaderButton(idFrom, false);
    VirtualFaderSlider* vfsFrom = getVirtualFaderSlider(idFrom, false);

    if (vfbFrom != nullptr || vfsFrom != nullptr) {
        copyElmt(idFrom, idTo);
        deleteElmt(idFrom);
    }
}

void VirtualFaderColGrid::copyElmt(int idFrom, int idTo)
{
    VirtualFaderButton* vfbFrom = getVirtualFaderButton(idFrom, false);
    VirtualFaderSlider* vfsFrom = getVirtualFaderSlider(idFrom, false);
    
    if (vfbFrom != nullptr || vfsFrom != nullptr) {
        var data = vfbFrom != nullptr ? vfbFrom->getJSONData() : vfsFrom->getJSONData();

        VirtualFaderButton* vfbTo = getVirtualFaderButton(idTo, true);
        VirtualFaderSlider* vfsTo = getVirtualFaderSlider(idTo, true);
        if (vfbTo != nullptr) { vfbTo->loadJSONData(data); }
        if (vfsTo != nullptr) { vfsTo->loadJSONData(data); }
    }

}

VirtualFaderCol* VirtualFaderColGrid::getVirtualFaderCol(int id, bool create)
{
    if (id < 1 || id > cols) { return nullptr; }
    VirtualFaderCol* vf = columnToVFC.getReference(id);
    String mode = "disabled";
    BKEngine* e = dynamic_cast<BKEngine*>(Engine::mainEngine);
    if (e->faderSelectionMode->getValueData() == "column") {
        mode = "column";
    }
    if (create) {
        vf = VirtualFaderColManager::getInstance()->addItem();
        vf->pageNumber->setValue(page);
        vf->colNumber->setValue(id);
        vf->fader.targetType->setValueWithData(mode);
        for (int i = 0; i < nRotaries; i++) {
            VirtualFaderSlider* temp = vf->rotaries.addItem();
            temp->targetType->setValueWithData(mode);
        }
        for (int i = 0; i < nAbove; i++) {
            VirtualFaderButton* temp = vf->aboveButtons.addItem();
            temp->targetType->setValueWithData(mode);
        }
        for (int i = 0; i < nBelow; i++) {
            VirtualFaderButton* temp = vf->belowButtons.addItem();
            temp->targetType->setValueWithData(mode);
        }
    }
    return vf;
}

VirtualFaderButton* VirtualFaderColGrid::getVirtualFaderButton(int index, bool create)
{
    TextButton* clicked = indexToButton.getReference(index);
    if (clicked == nullptr) {return nullptr; }
    VirtualFaderButton* vfb = nullptr;
    if (buttonToVFB.contains(clicked) && buttonToVFB.getReference(clicked) != nullptr) {
        return buttonToVFB.getReference(clicked);
    } else {
        int col = buttonColumnIndex.getReference(clicked);
        VirtualFaderCol* vfc = getVirtualFaderCol(col, true);
        int maxBtn = nRotaries + nAbove + nBelow +1;
        int n = index % maxBtn;
        if (n < nRotaries) {
            return nullptr;
        }
        else if (n < nRotaries + nAbove) {
            n -= nRotaries;
            while (vfc->aboveButtons.items.size() < n) {
                vfc->aboveButtons.addItem();
            }
            vfb = vfc->aboveButtons.items[n];
        }
        else if (n == nRotaries + nAbove) {
            return nullptr;
        }
        else {
            n -= nAbove + nRotaries + 1;
            while (vfc->belowButtons.items.size() < n) {
                vfc->belowButtons.addItem();
            }
            vfb = vfc->belowButtons.items[n];
        }
        buttonToVFB.set(clicked, vfb);
    }
    return vfb;
}

VirtualFaderSlider* VirtualFaderColGrid::getVirtualFaderSlider(int index, bool create)
{
    Slider* clicked = indexToSlider.getReference(index);
    if (clicked == nullptr) { return nullptr; }
    VirtualFaderSlider* vfs = nullptr;
    if (sliderToVFS.contains(clicked) && sliderToVFS.getReference(clicked) != nullptr) {
        return sliderToVFS.getReference(clicked);
    } else {
        int col = sliderColumnIndex.getReference(clicked);
        VirtualFaderCol* vfc = getVirtualFaderCol(col, true);
        int maxBtn = nRotaries + nAbove + nBelow + 1;
        int n = index % maxBtn;
        if (n < nRotaries) {
            while (vfc->rotaries.items.size() < n) {
                vfc->rotaries.addItem();
            }
            vfs = vfc->rotaries.items[n];
        }
        else if (n < nRotaries + nAbove) {
            vfs = nullptr;
        }
        else if (n == nRotaries + nAbove) {
            vfs = &vfc->fader;
        }
        else {
            vfs = nullptr;
        }
        sliderToVFS.set(clicked, vfs);
    }
    return vfs;
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
            v = s->getTargetValue();
            s->feedback(v, "");
        }
        faders[i]->setValue(v, juce::dontSendNotification);

        for (int r = 0; r < nRotaries; r++) {
            s = sliderToVFS.getReference(rotaries[i]->getRawDataPointer()[r]);
            v = 0;
            if (s != nullptr) {
                v = s->getTargetValue();
                s->feedback(v, "");
            }
            rotaries[i]->getRawDataPointer()[r]->setValue(v, juce::dontSendNotification);

        }
    }
}

