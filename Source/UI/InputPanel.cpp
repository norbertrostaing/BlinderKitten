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

//==============================================================================
InputPanelUI::InputPanelUI(const String& contentName):
    ShapeShifterContent(InputPanel::getInstance(), contentName)
{
    
}

InputPanelUI::~InputPanelUI()
{
}

juce_ImplementSingleton(InputPanel);

InputPanel::InputPanel()
{
    addAndMakeVisible(btnClear); btnClear.setButtonText("Clear"); btnClear.addListener(this); btnClear.setWantsKeyboardFocus(false);
    addAndMakeVisible(btnEdit); btnEdit.setButtonText("Edit"); btnEdit.addListener(this); btnEdit.setWantsKeyboardFocus(false);
    addAndMakeVisible(btnRecord); btnRecord.setButtonText("Record"); btnRecord.addListener(this); btnRecord.setWantsKeyboardFocus(false);
    addAndMakeVisible(btnDelete); btnDelete.setButtonText("Delete"); btnDelete.addListener(this); btnDelete.setWantsKeyboardFocus(false);
    addAndMakeVisible(btnCopy); btnCopy.setButtonText("Copy"); btnCopy.addListener(this); btnCopy.setWantsKeyboardFocus(false);
    addAndMakeVisible(btnFixture); btnFixture.setButtonText("Fixture"); btnFixture.addListener(this); btnFixture.setWantsKeyboardFocus(false);
    addAndMakeVisible(btnGroup); btnGroup.setButtonText("Group"); btnGroup.addListener(this); btnGroup.setWantsKeyboardFocus(false);
    addAndMakeVisible(btnSub); btnSub.setButtonText("SubFixture"); btnSub.addListener(this); btnSub.setWantsKeyboardFocus(false);
    addAndMakeVisible(btnPreset); btnPreset.setButtonText("Preset"); btnPreset.addListener(this); btnPreset.setWantsKeyboardFocus(false);
    addAndMakeVisible(btnCuelist); btnCuelist.setButtonText("Cuelist"); btnCuelist.addListener(this); btnCuelist.setWantsKeyboardFocus(false);
    addAndMakeVisible(btnCue); btnCue.setButtonText("Cue"); btnCue.addListener(this); btnCue.setWantsKeyboardFocus(false);
    addAndMakeVisible(btnEffect); btnEffect.setButtonText("Cuelist"); btnEffect.addListener(this); btnEffect.setWantsKeyboardFocus(false);
    addAndMakeVisible(btnEffect); btnEffect.setButtonText("Effect"); btnEffect.addListener(this); btnEffect.setWantsKeyboardFocus(false);
    addAndMakeVisible(btnCarousel); btnCarousel.setButtonText("Carousel"); btnCarousel.addListener(this); btnCarousel.setWantsKeyboardFocus(false);
    addAndMakeVisible(btn1); btn1.setButtonText("1"); btn1.addListener(this); btn1.setWantsKeyboardFocus(false);
    addAndMakeVisible(btn2); btn2.setButtonText("2"); btn2.addListener(this); btn2.setWantsKeyboardFocus(false);
    addAndMakeVisible(btn3); btn3.setButtonText("3"); btn3.addListener(this); btn3.setWantsKeyboardFocus(false);
    addAndMakeVisible(btn4); btn4.setButtonText("4"); btn4.addListener(this); btn4.setWantsKeyboardFocus(false);
    addAndMakeVisible(btn5); btn5.setButtonText("5"); btn5.addListener(this); btn5.setWantsKeyboardFocus(false);
    addAndMakeVisible(btn6); btn6.setButtonText("6"); btn6.addListener(this); btn6.setWantsKeyboardFocus(false);
    addAndMakeVisible(btn7); btn7.setButtonText("7"); btn7.addListener(this); btn7.setWantsKeyboardFocus(false);
    addAndMakeVisible(btn8); btn8.setButtonText("8"); btn8.addListener(this); btn8.setWantsKeyboardFocus(false);
    addAndMakeVisible(btn9); btn9.setButtonText("9"); btn9.addListener(this); btn9.setWantsKeyboardFocus(false);
    addAndMakeVisible(btn0); btn0.setButtonText("0"); btn0.addListener(this); btn0.setWantsKeyboardFocus(false);
    addAndMakeVisible(btnPlus); btnPlus.setButtonText("+"); btnPlus.addListener(this); btnPlus.setWantsKeyboardFocus(false);
    addAndMakeVisible(btnMinus); btnMinus.setButtonText("-"); btnMinus.addListener(this); btnMinus.setWantsKeyboardFocus(false);
    addAndMakeVisible(btnDot); btnDot.setButtonText("."); btnDot.addListener(this); btnDot.setWantsKeyboardFocus(false);
    addAndMakeVisible(btnThru); btnThru.setButtonText("Thru"); btnThru.addListener(this); btnThru.setWantsKeyboardFocus(false);
    addAndMakeVisible(btnBackspace); btnBackspace.setButtonText("<-"); btnBackspace.addListener(this); btnBackspace.setWantsKeyboardFocus(false);
    addAndMakeVisible(btnEnter); btnEnter.setButtonText("Enter"); btnEnter.addListener(this); btnEnter.setWantsKeyboardFocus(false);
    addAndMakeVisible(btnAssistant); btnAssistant.setButtonText("Assistant"); btnAssistant.addListener(this); btnAssistant.setWantsKeyboardFocus(false);

    addAndMakeVisible(grandMaster); 
    grandMaster.setSliderStyle(Slider::LinearBarVertical);
    grandMaster.setSliderSnapsToMousePosition(false);
    grandMaster.setRange(0, 1);
    grandMaster.setValue(1, juce::dontSendNotification);
    grandMaster.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    grandMaster.addListener(this);
    grandMaster.setWantsKeyboardFocus(false);

    addAndMakeVisible(blackoutBtn);
    blackoutBtn.setButtonText("Black Out");
    blackoutBtn.addListener(this);
    blackoutBtn.setToggleable(true);
    blackoutBtn.setClickingTogglesState(true);
    blackoutBtn.setWantsKeyboardFocus(false);

    addAndMakeVisible(remoteInLockBtn);
    remoteInLockBtn.setButtonText("MIDI Lock");
    remoteInLockBtn.addListener(this);
    remoteInLockBtn.setToggleable(true);
    remoteInLockBtn.setClickingTogglesState(true);
    remoteInLockBtn.setWantsKeyboardFocus(false);
    remoteInLockBtn.onClick = [this]() {
        if (!remoteInLockBtn.getToggleState()) {
            VirtualButtonGrid::getInstance()->goToPage(VirtualButtonGrid::getInstance()->page);
            VirtualFaderColGrid::getInstance()->goToPage(VirtualFaderColGrid::getInstance()->page);
        }
    };

}

InputPanel::~InputPanel()
{
}

void InputPanel::buttonClicked(juce::Button* button) {
    UserInputManager::getInstance()->resetFocus();
    ModifierKeys k = ModifierKeys::getCurrentModifiersRealtime();
    if (button == &btnAssistant){
        Assistant::getInstance()->selectThis();
    }
    else if (button == &btnBackspace) {
        UserInputManager::getInstance()->processInput("backspace");
    }
    else if (button == &btnClear && k.isShiftDown()) {
        UserInputManager::getInstance()->processInput("clearall");
    }
    else if (button == &blackoutBtn) {
        Brain::getInstance()->grandMasterChanged();
        UserInputManager::getInstance()->feedback("/blackout", blackoutBtn.getToggleState() ? 1 : 0, "");
    }
    else if (button == &btnCopy) {
        UserInputManager::getInstance()->processInput("copy");
    }
    else if (button == &btnRecord) {
        UserInputManager::getInstance()->processInput("record");
    }
    else if (button == &btnEdit) {
        UserInputManager::getInstance()->processInput("edit");
    }
    else {
        UserInputManager::getInstance()->processInput(button->getButtonText());
    }
}

void InputPanel::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

}

void InputPanel::resized()
{
    
    bool fixedSize = false;

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

    btnAssistant.setBounds  (0,0,lg, h);
    btnClear.setBounds      (lg+firstLineMargin, 0 * r, lg, h);
    btnRecord.setBounds     (2*(lg + firstLineMargin), 0 * r, lg, h);

    btnEdit.setBounds       (0 * lg + p3, 1 * r, lg, h);
    btnCopy.setBounds       (0 * lg + p3, 2 * r, lg, h);
    btnDelete.setBounds     (0 * lg + p3, 3 * r, lg, h);

    btnFixture.setBounds    (0 * lg, 1 * r, lg, h);
    btnSub.setBounds        (1 * lg, 1 * r, lg, h);
    btnGroup.setBounds      (0 * lg, 2 * r, lg, h);
    btnPreset.setBounds     (1 * lg, 2 * r, lg, h);

    btnCuelist.setBounds    (0 * lg, 3 * r, lg, h);
    btnCue.setBounds        (1 * lg, 3 * r, lg, h);
    btnEffect.setBounds     (0 * lg, 4 * r, lg, h);
    btnCarousel.setBounds   (1 * lg, 4 * r, lg, h);

    btn7.setBounds          (0 * sm + p2, 1 * r, sm, h);
    btn8.setBounds          (1 * sm + p2, 1 * r, sm, h);
    btn9.setBounds          (2 * sm + p2, 1 * r, sm, h);
    btnPlus.setBounds       (3 * sm + p2, 1 * r, sm, h);
    btn4.setBounds          (0 * sm + p2, 2 * r, sm, h);
    btn5.setBounds          (1 * sm + p2, 2 * r, sm, h);
    btn6.setBounds          (2 * sm + p2, 2 * r, sm, h);
    btnMinus.setBounds      (3 * sm + p2, 2 * r, sm, h);
    btn1.setBounds          (0 * sm + p2, 3 * r, sm, h);
    btn2.setBounds          (1 * sm + p2, 3 * r, sm, h);
    btn3.setBounds          (2 * sm + p2, 3 * r, sm, h);
    btnThru.setBounds       (3 * sm + p2, 3 * r, sm, h);
    btn0.setBounds          (0 * sm + p2, 4 * r, sm, h);
    btnBackspace.setBounds  (1 * sm + p2, 4 * r, sm, h);
    btnEnter.setBounds      (2 * sm + p2, 4 * r, sm*2, h);

    remoteInLockBtn.setBounds(0 * lg + p3 + 0*(lg / 2), 4 * r, lg / 2, h);
    blackoutBtn.setBounds(0 * lg + p3 + 1*(lg / 2), 4 * r, lg / 2, h);
    grandMaster.setBounds(p3 + 2*(lg / 2), 0, sm, 5*h);


    // This method is where you should set the bounds of any child
    // components that your component contains..
}

void InputPanel::sliderValueChanged(Slider* slider)
{
    if (slider == &grandMaster) {
        setGrandMaster(-1, "");
    }
    //UserInputManager::getInstance()->encoderValueChanged(index, slider->getValue());

}

void InputPanel::setGrandMaster(float value, String origin)
{
    if (value >= 0) {
        grandMaster.setValue(value, juce::dontSendNotification);
    }
    Brain::getInstance()->grandMasterChanged();
    UserInputManager::getInstance()->feedback("/grandmaster", grandMaster.getValue(), origin);
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
        if (action == "loadcontent") editText = "Load content";
        if (action == "update") recordText = "Update";
        if (action == "replace") recordText = "Replace";

    }

    btnCopy.setButtonText(copyText);
    btnEdit.setButtonText(editText);
    btnRecord.setButtonText(recordText);
}
