/*
  ==============================================================================

    InputPanel.cpp
    Created: 19 Feb 2022 12:19:42am
    Author:  No

  ==============================================================================
*/

#include <JuceHeader.h>
#include "InputPanel.h"
#include "UserInputManager.h"
#include "BKEngine.h"
#include "Definitions/Assistant/Assistant.h"
#include "UI/VirtualButtons/VirtualButtonGrid.h"
#include "UI/VirtualFaders/VirtualFaderColGrid.h"
#include "Definitions/Programmer/Programmer.h"

//==============================================================================
InputPanelUI::InputPanelUI(const String& contentName):
    ShapeShifterContent(InputPanel::getInstance(), contentName)
{
    
}

InputPanelUI::~InputPanelUI()
{
}

juce_ImplementSingleton(InputPanel);

InputPanel::InputPanel():
    ControllableContainer("Input Panel")
{
    paramClear = addTrigger("Clear", "Clear the current command"); btnClear = paramClear->createButtonUI(); addAndMakeVisible(btnClear);   
    btnClear->customBGColor = Colour(59, 59, 59); btnClear->useCustomBGColor = true; 
    paramEdit = addTrigger("Edit", "Edit an item"); btnEdit = paramEdit->createButtonUI(); addAndMakeVisible(btnEdit);  
    btnEdit->customBGColor = Colour(59, 59, 59); btnEdit->useCustomBGColor = true;
    paramRecord = addTrigger("Record", "Record, merge or replace"); btnRecord = paramRecord->createButtonUI(); addAndMakeVisible(btnRecord);  
    btnRecord->customBGColor = Colour(59, 59, 59); btnRecord->useCustomBGColor = true;
    paramDelete = addTrigger("Delete", "Delete an item"); btnDelete = paramDelete->createButtonUI(); addAndMakeVisible(btnDelete);  
    btnDelete->customBGColor = Colour(59, 59, 59); btnDelete->useCustomBGColor = true;
    paramCopy = addTrigger("Copy", "Copy or move an item"); btnCopy = paramCopy->createButtonUI(); addAndMakeVisible(btnCopy);  
    btnCopy->customBGColor = Colour(59, 59, 59); btnCopy->useCustomBGColor = true;
    paramFixture = addTrigger("Fixture", "Select a fixture"); btnFixture = paramFixture->createButtonUI(); addAndMakeVisible(btnFixture);  
    btnFixture->customBGColor = Colour(59, 59, 59); btnFixture->useCustomBGColor = true;
    paramGroup = addTrigger("Group", "Select a group"); btnGroup = paramGroup->createButtonUI(); addAndMakeVisible(btnGroup);  
    btnGroup->customBGColor = Colour(59, 59, 59); btnGroup->useCustomBGColor = true;
    paramSub = addTrigger("Subfixture", "Select a subfixture"); btnSub = paramSub->createButtonUI(); addAndMakeVisible(btnSub);  
    btnSub->customBGColor = Colour(59, 59, 59); btnSub->useCustomBGColor = true;
    paramPreset = addTrigger("Preset", "Select a preset"); btnPreset = paramPreset->createButtonUI(); addAndMakeVisible(btnPreset);  
    btnPreset->customBGColor = Colour(59, 59, 59); btnPreset->useCustomBGColor = true;
    paramCuelist = addTrigger("Cuelist", "Select a cuelist"); btnCuelist = paramCuelist->createButtonUI(); addAndMakeVisible(btnCuelist);  
    btnCuelist->customBGColor = Colour(59, 59, 59); btnCuelist->useCustomBGColor = true;
    paramCue = addTrigger("Cue", "Select a cue"); btnCue = paramCue->createButtonUI(); addAndMakeVisible(btnCue);  
    btnCue->customBGColor = Colour(59, 59, 59); btnCue->useCustomBGColor = true;
    paramEffect = addTrigger("Effect", "Select an effect"); btnEffect = paramEffect->createButtonUI(); addAndMakeVisible(btnEffect);  
    btnEffect->customBGColor = Colour(59, 59, 59); btnEffect->useCustomBGColor = true;
    paramCarousel = addTrigger("Carousel", "Select a carousel"); btnCarousel = paramCarousel->createButtonUI(); addAndMakeVisible(btnCarousel);  
    btnCarousel->customBGColor = Colour(59, 59, 59); btnCarousel->useCustomBGColor = true;
    param1 = addTrigger("1", "1"); btn1 = param1->createButtonUI(); addAndMakeVisible(btn1);  
    btn1->customBGColor = Colour(59, 59, 59); btn1->useCustomBGColor = true;
    param2 = addTrigger("2", "2"); btn2 = param2->createButtonUI(); addAndMakeVisible(btn2);  
    btn2->customBGColor = Colour(59, 59, 59); btn2->useCustomBGColor = true;
    param3 = addTrigger("3", "3"); btn3 = param3->createButtonUI(); addAndMakeVisible(btn3);  
    btn3->customBGColor = Colour(59, 59, 59); btn3->useCustomBGColor = true;
    param4 = addTrigger("4", "4"); btn4 = param4->createButtonUI(); addAndMakeVisible(btn4);  
    btn4->customBGColor = Colour(59, 59, 59); btn4->useCustomBGColor = true;
    param5 = addTrigger("5", "5"); btn5 = param5->createButtonUI(); addAndMakeVisible(btn5);  
    btn5->customBGColor = Colour(59, 59, 59); btn5->useCustomBGColor = true;
    param6 = addTrigger("6", "6"); btn6 = param6->createButtonUI(); addAndMakeVisible(btn6);  
    btn6->customBGColor = Colour(59, 59, 59); btn6->useCustomBGColor = true;
    param7 = addTrigger("7", "7"); btn7 = param7->createButtonUI(); addAndMakeVisible(btn7);  
    btn7->customBGColor = Colour(59, 59, 59); btn7->useCustomBGColor = true;
    param8 = addTrigger("8", "8"); btn8 = param8->createButtonUI(); addAndMakeVisible(btn8);  
    btn8->customBGColor = Colour(59, 59, 59); btn8->useCustomBGColor = true;
    param9 = addTrigger("9", "9"); btn9 = param9->createButtonUI(); addAndMakeVisible(btn9);  
    btn9->customBGColor = Colour(59, 59, 59); btn9->useCustomBGColor = true;
    param0 = addTrigger("0", "0"); btn0 = param0->createButtonUI(); addAndMakeVisible(btn0);  
    btn0->customBGColor = Colour(59, 59, 59); btn0->useCustomBGColor = true;
    paramPlus = addTrigger("+", "+"); paramPlus->setCustomShortName("plus");  btnPlus = paramPlus->createButtonUI(); addAndMakeVisible(btnPlus);
    btnPlus->customBGColor = Colour(59, 59, 59); btnPlus->useCustomBGColor = true;
    paramMinus = addTrigger("-", "-"); paramMinus->setCustomShortName("minux");  btnMinus = paramMinus->createButtonUI(); addAndMakeVisible(btnMinus);
    btnMinus->customBGColor = Colour(59, 59, 59); btnMinus->useCustomBGColor = true;
    paramDot = addTrigger(".", "."); paramDot->setCustomShortName("dot"); btnDot = paramDot->createButtonUI(); addAndMakeVisible(btnDot);
    btnDot->customBGColor = Colour(59, 59, 59); btnDot->useCustomBGColor = true;
    paramThru = addTrigger("Thru", "Thru"); btnThru = paramThru->createButtonUI(); addAndMakeVisible(btnThru);  
    btnThru->customBGColor = Colour(59, 59, 59); btnThru->useCustomBGColor = true;
    paramBackspace = addTrigger("<-", "Backspace"); paramBackspace->setCustomShortName("backspace");  btnBackspace = paramBackspace->createButtonUI(); addAndMakeVisible(btnBackspace);
    btnBackspace->customBGColor = Colour(59, 59, 59); btnBackspace->useCustomBGColor = true;
    paramEnter = addTrigger("Enter", "Enter"); btnEnter = paramEnter->createButtonUI(); addAndMakeVisible(btnEnter);  
    btnEnter->customBGColor = Colour(59, 59, 59); btnEnter->useCustomBGColor = true;
    paramAssistant = addTrigger("Assistant", "Need an assistant ?"); btnAssistant = paramAssistant->createButtonUI(); addAndMakeVisible(btnAssistant);  
    btnAssistant->customBGColor = Colour(59, 59, 59); btnAssistant->useCustomBGColor = true;

    paramGrandMaster = addFloatParameter("Grand Master", "Grand Master",1,0,1); sliderGrandMaster = paramGrandMaster->createSlider(); addAndMakeVisible(sliderGrandMaster);
    sliderGrandMaster->orientation = FloatSliderUI::VERTICAL;
    sliderGrandMaster->showValue = false;

    paramBlackOut = addBoolParameter("BlackOut", "It's like set the grand master to 0", false); btnBlackout = paramBlackOut->createButtonToggle(); addAndMakeVisible(btnBlackout);  
    btnBlackout->customBGColor = Colour(59, 59, 59); btnBlackout->useCustomBGColor = true;

    paramInLock = addBoolParameter("Remote lock", "Disable all midi inputs", false); btnInLock = paramInLock->createButtonToggle(); addAndMakeVisible(btnInLock);  
    btnInLock->customBGColor = Colour(59, 59, 59); btnInLock->useCustomBGColor = true;
}

InputPanel::~InputPanel()
{
    delete btnFixture;
    delete btnGroup;
    delete btnSub;
    delete btnCuelist;
    delete btnCue;
    delete btnPreset;
    delete btnEffect;
    delete btnCarousel;
    delete btn1;
    delete btn2;
    delete btn3;
    delete btn4;
    delete btn5;
    delete btn6;
    delete btn7;
    delete btn8;
    delete btn9;
    delete btn0;
    delete btnPlus;
    delete btnMinus;
    delete btnDot;
    delete btnThru;
    delete btnBackspace;
    delete btnEnter;
    delete btnClear;
    delete btnRecord;
    delete btnEdit;
    delete btnCopy;
    delete btnDelete;
    delete btnAssistant;
    delete btnBlackout;
    delete btnInLock;
    delete sliderGrandMaster;
}

void InputPanel::paint (juce::Graphics& g)
{
}

void InputPanel::resized()
{
    float scaleX = 1;
    float scaleY = 1;

    scaleX = getWidth() / 600.;
    scaleY = getHeight() / 200.;
    int nChildren =getNumChildComponents ();
    for (int i = 0; i < nChildren; i++) {
        //getChildComponent(i)->setTransform(AffineTransform::scale(scaleX, scaleY));

    }

    int margin = 20 * scaleX;
    int h = 40 * scaleY;
    int r = 40 * scaleY;
    int sm = 40 * scaleX;
    int lg = 120 * scaleX;
    int p2 = 2*lg + margin;
    int p3 = p2 + (4 * sm) + margin;
    // int mid = (p3 + lg)/2;

    int tot = p3+lg;
    int firstLineMargin = (tot-(3*lg))/2;

    btnAssistant -> setBounds  (0,0,lg, h);
    btnClear -> setBounds      (lg+firstLineMargin, 0 * r, lg, h);
    btnRecord -> setBounds     (2*(lg + firstLineMargin), 0 * r, lg, h);

    btnEdit -> setBounds       (0 * lg + p3, 1 * r, lg, h);
    btnCopy -> setBounds       (0 * lg + p3, 2 * r, lg, h);
    btnDelete -> setBounds     (0 * lg + p3, 3 * r, lg, h);

    btnFixture -> setBounds    (0 * lg, 1 * r, lg, h);
    btnSub -> setBounds        (1 * lg, 1 * r, lg, h);
    btnGroup -> setBounds      (0 * lg, 2 * r, lg, h);
    btnPreset -> setBounds     (1 * lg, 2 * r, lg, h);

    btnCuelist -> setBounds    (0 * lg, 3 * r, lg, h);
    btnCue -> setBounds        (1 * lg, 3 * r, lg, h);
    btnEffect -> setBounds     (0 * lg, 4 * r, lg, h);
    btnCarousel -> setBounds   (1 * lg, 4 * r, lg, h);

    btn7 -> setBounds          (0 * sm + p2, 1 * r, sm, h);
    btn8 -> setBounds          (1 * sm + p2, 1 * r, sm, h);
    btn9 -> setBounds          (2 * sm + p2, 1 * r, sm, h);
    btnPlus -> setBounds       (3 * sm + p2, 1 * r, sm, h);
    btn4 -> setBounds          (0 * sm + p2, 2 * r, sm, h);
    btn5 -> setBounds          (1 * sm + p2, 2 * r, sm, h);
    btn6 -> setBounds          (2 * sm + p2, 2 * r, sm, h);
    btnMinus -> setBounds      (3 * sm + p2, 2 * r, sm, h);
    btn1 -> setBounds          (0 * sm + p2, 3 * r, sm, h);
    btn2 -> setBounds          (1 * sm + p2, 3 * r, sm, h);
    btn3 -> setBounds          (2 * sm + p2, 3 * r, sm, h);
    btnThru -> setBounds       (3 * sm + p2, 3 * r, sm, h);
    btn0 -> setBounds          (0 * sm + p2, 4 * r, sm, h);
    btnBackspace -> setBounds  (1 * sm + p2, 4 * r, sm, h);
    btnDot->setBounds          (2 * sm + p2, 4 * r, sm, h);
    btnEnter->setBounds        (3 * sm + p2, 4 * r, sm, h);

    btnInLock -> setBounds(0 * lg + p3 + 0*(lg / 2), 4 * r, lg / 2, h);
    btnBlackout -> setBounds(0 * lg + p3 + 1*(lg / 2), 4 * r, lg / 2, h);
    sliderGrandMaster -> setBounds(p3 + 2*(lg / 2), 0, sm, 5*h);


    // This method is where you should set the bounds of any child
    // components that your component contains..
}

void InputPanel::triggerTriggered(Trigger* t)
{
    ModifierKeys k = ModifierKeys::getCurrentModifiersRealtime();

    if (t == paramAssistant){
        Assistant::getInstance()->selectThis();
    }
    else if (t == paramBackspace) {
        UserInputManager::getInstance()->processInput("backspace");
    }
    else if (t == paramClear && k.isShiftDown()) {
        UserInputManager::getInstance()->processInput("clearall");
    }
    else if (t == paramCopy) {
        UserInputManager::getInstance()->processInput("copy");
    }
    else if (t == paramRecord) {
        UserInputManager::getInstance()->processInput("record");
    }
    else if (t == paramEdit) {
        UserInputManager::getInstance()->processInput("edit");
    }
    else {
        UserInputManager::getInstance()->processInput(t->niceName);
    }

}

void InputPanel::parameterValueChanged(Parameter* p)
{
    if (p == paramBlackOut) {
        Brain::getInstance()->grandMasterChanged();
        UserInputManager::getInstance()->feedback("/blackout", paramBlackOut->boolValue() ? 1 : 0, "");
    }
    if (p == paramGrandMaster) {
        setGrandMaster(-1, "");
    }
}

void InputPanel::setGrandMaster(float value, String origin)
{
    if (value >= 0) {
        paramGrandMaster->setValue(value);
    }
    Brain::getInstance()->grandMasterChanged();
    UserInputManager::getInstance()->feedback("/grandmaster", paramGrandMaster->floatValue(), origin);
}

void InputPanel::updateButtonsText()
{
    String copyText = "Copy";
    String editText = "Edit";
    String recordText = "Record";
    Programmer* p = UserInputManager::getInstance()->getProgrammer(false);
    if (p != nullptr) {
        String action = p->cliActionType->getValueData().toString();
        if (action == "move") copyText = "Move";
        if (action == "setanother") copyText = "Set another";
        if (action == "loadcontent") editText = "Load content";
        if (action == "merge") recordText = "Merge";
        if (action == "replace") recordText = "Replace";
    }

    paramRecord->setNiceName(recordText);
    paramEdit->setNiceName(editText);
    paramCopy->setNiceName(copyText);
}
