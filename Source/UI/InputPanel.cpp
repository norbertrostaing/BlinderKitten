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
    addAndMakeVisible(btnClear); btnClear.setButtonText("Clear"); btnClear.addListener(this);
    addAndMakeVisible(btnEdit); btnEdit.setButtonText("Edit"); btnEdit.addListener(this);
    addAndMakeVisible(btnRecord); btnRecord.setButtonText("Record"); btnRecord.addListener(this);
    addAndMakeVisible(btnDelete); btnDelete.setButtonText("Delete"); btnDelete.addListener(this);
    addAndMakeVisible(btnCopy); btnCopy.setButtonText("Copy"); btnCopy.addListener(this);
    addAndMakeVisible(btnFixture); btnFixture.setButtonText("Fixture"); btnFixture.addListener(this);
    addAndMakeVisible(btnGroup); btnGroup.setButtonText("Group"); btnGroup.addListener(this);
    addAndMakeVisible(btnSub); btnSub.setButtonText("SubFixture"); btnSub.addListener(this);
    addAndMakeVisible(btnPreset); btnPreset.setButtonText("Preset"); btnPreset.addListener(this);
    addAndMakeVisible(btnCuelist); btnCuelist.setButtonText("Cuelist"); btnCuelist.addListener(this);
    addAndMakeVisible(btnCue); btnCue.setButtonText("Cue"); btnCue.addListener(this);
    addAndMakeVisible(btnEffect); btnEffect.setButtonText("Cuelist"); btnEffect.addListener(this);
    addAndMakeVisible(btnEffect); btnEffect.setButtonText("Effect"); btnEffect.addListener(this);
    addAndMakeVisible(btnCarousel); btnCarousel.setButtonText("Carousel"); btnCarousel.addListener(this);
    addAndMakeVisible(btn1); btn1.setButtonText("1"); btn1.addListener(this);
    addAndMakeVisible(btn2); btn2.setButtonText("2"); btn2.addListener(this);
    addAndMakeVisible(btn3); btn3.setButtonText("3"); btn3.addListener(this);
    addAndMakeVisible(btn4); btn4.setButtonText("4"); btn4.addListener(this);
    addAndMakeVisible(btn5); btn5.setButtonText("5"); btn5.addListener(this);
    addAndMakeVisible(btn6); btn6.setButtonText("6"); btn6.addListener(this);
    addAndMakeVisible(btn7); btn7.setButtonText("7"); btn7.addListener(this);
    addAndMakeVisible(btn8); btn8.setButtonText("8"); btn8.addListener(this);
    addAndMakeVisible(btn9); btn9.setButtonText("9"); btn9.addListener(this);
    addAndMakeVisible(btn0); btn0.setButtonText("0"); btn0.addListener(this);
    addAndMakeVisible(btnPlus); btnPlus.setButtonText("+"); btnPlus.addListener(this);
    addAndMakeVisible(btnMinus); btnMinus.setButtonText("-"); btnMinus.addListener(this);
    addAndMakeVisible(btnDot); btnDot.setButtonText("."); btnDot.addListener(this);
    addAndMakeVisible(btnThru); btnThru.setButtonText("Thru"); btnThru.addListener(this);
    addAndMakeVisible(btnBackspace); btnBackspace.setButtonText("<-"); btnBackspace.addListener(this);
    addAndMakeVisible(btnEnter); btnEnter.setButtonText("Enter"); btnEnter.addListener(this);
    addAndMakeVisible(btnAssistant); btnAssistant.setButtonText("Assistant"); btnAssistant.addListener(this);

    addAndMakeVisible(grandMaster); 
    grandMaster.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    grandMaster.setRange(0, 1);
    grandMaster.setValue(1, juce::dontSendNotification);
    grandMaster.setColour(Slider::rotarySliderFillColourId, Colour(63, 63, 63));
    grandMaster.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    grandMaster.addListener(this);
    grandMaster.setWantsKeyboardFocus(false);


}

InputPanel::~InputPanel()
{
}

void InputPanel::buttonClicked(juce::Button* button) {
    if (button == &btnAssistant){
        Assistant::getInstance()->selectThis();
    }
    else if (button == &btnBackspace) {
        UserInputManager::getInstance()->processInput("backspace");
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
    
    float scale = 1;
    if (engine != nullptr && engine->panelScale != nullptr) {
        scale = engine->panelScale->getValue();
    }
    int nChildren =getNumChildComponents ();
    for (int i = 0; i < nChildren; i++) {
        getChildComponent(i)->setTransform(AffineTransform::scale(scale));

    }


    int margin = 20;
    int h = 40;
    int r = 40;
    int sm = 40;
    int lg = 120;
    int p2 = 2*lg + margin;
    int p3 = p2 + (4 * sm) + margin;
    int mid = (p3 + lg)/2;

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

    grandMaster.setBounds   (0 * lg + p3, 4 * r, lg, h);


    // This method is where you should set the bounds of any child
    // components that your component contains..
}

void InputPanel::sliderValueChanged(Slider* slider)
{
    if (slider == &grandMaster) {
        Brain::getInstance()->grandMasterChanged();
    }
    //UserInputManager::getInstance()->encoderValueChanged(index, slider->getValue());

}
