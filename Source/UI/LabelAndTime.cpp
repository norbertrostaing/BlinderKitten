/*
  ==============================================================================

    LabelAndTime.cpp
    Created: 4 Mar 2022 1:15:59pm
    Author:  No

  ==============================================================================
*/

#include <JuceHeader.h>
#include "LabelAndTime.h"

#include "Definitions//Cuelist/Cuelist.h"
#include "UserInputManager.h"
#include "BKEngine.h"

juce_ImplementSingleton(LabelAndTimeWindow);

//==============================================================================
LabelAndTimeWindow::LabelAndTimeWindow():
    filter(0, "0123456789.+-/*")
{
    setLookAndFeel(&getLookAndFeel());
    addAndMakeVisible(nameLabel);
    nameLabel.setText("New name", juce::dontSendNotification);
    nameLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(htpUpLabel);
    htpUpLabel.setText("HTP Up", juce::dontSendNotification);
    htpUpLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(htpDownLabel);
    htpDownLabel.setText("HTP Down", juce::dontSendNotification);
    htpDownLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(ltpLabel);
    ltpLabel.setText("LTP", juce::dontSendNotification);
    ltpLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(delayLabel);
    delayLabel.setText("Delay", juce::dontSendNotification);
    delayLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(fadeLabel);
    fadeLabel.setText("Fade", juce::dontSendNotification);
    fadeLabel.setJustificationType(Justification::centred);

    Colour back(31,31,31);
    Colour text(204,204,204);

    addAndMakeVisible(nameEdit);
    nameEdit.onReturnKey = [this](){validBtn.triggerClick();};
    nameEdit.setColour(TextEditor::backgroundColourId, back);
    nameEdit.setColour(TextEditor::textColourId, text);

    addAndMakeVisible(htpUpDelayEdit);
    htpUpDelayEdit.onReturnKey = [this]() {validBtn.triggerClick(); };
    htpUpDelayEdit.setInputFilter(&filter, false);
    htpUpDelayEdit.setColour(TextEditor::backgroundColourId, back);
    htpUpDelayEdit.setColour(TextEditor::textColourId, text);

    addAndMakeVisible(htpDownDelayEdit);
    htpDownDelayEdit.onReturnKey = [this]() {validBtn.triggerClick(); };
    htpDownDelayEdit.setInputFilter(&filter, false);
    htpDownDelayEdit.setColour(TextEditor::backgroundColourId, back);
    htpDownDelayEdit.setColour(TextEditor::textColourId, text);

    addAndMakeVisible(ltpDelayEdit);
    ltpDelayEdit.onReturnKey = [this]() {validBtn.triggerClick(); };
    ltpDelayEdit.setInputFilter(&filter, false);
    ltpDelayEdit.setColour(TextEditor::backgroundColourId, back);
    ltpDelayEdit.setColour(TextEditor::textColourId, text);

    addAndMakeVisible(htpUpFadeEdit);
    htpUpFadeEdit.onReturnKey = [this]() {validBtn.triggerClick(); };
    htpUpFadeEdit.setInputFilter(&filter, false);
    htpUpFadeEdit.setColour(TextEditor::backgroundColourId, back);
    htpUpFadeEdit.setColour(TextEditor::textColourId, text);

    addAndMakeVisible(htpDownFadeEdit);
    htpDownFadeEdit.onReturnKey = [this]() {validBtn.triggerClick(); };
    htpDownFadeEdit.setInputFilter(&filter, false);
    htpDownFadeEdit.setColour(TextEditor::backgroundColourId, back);
    htpDownFadeEdit.setColour(TextEditor::textColourId, text);

    addAndMakeVisible(ltpFadeEdit);
    ltpFadeEdit.onReturnKey = [this]() {validBtn.triggerClick(); };
    ltpFadeEdit.setInputFilter(&filter, false);
    ltpFadeEdit.setColour(TextEditor::backgroundColourId, back);
    ltpFadeEdit.setColour(TextEditor::textColourId, text);


    addAndMakeVisible(validBtn);
    validBtn.setButtonText("Apply");
    validBtn.addListener(this);
    addAndMakeVisible(exitBtn);
    exitBtn.setButtonText("Cancel");
    exitBtn.addListener(this);

}

LabelAndTimeWindow::~LabelAndTimeWindow()
{
}

void LabelAndTimeWindow::paint (juce::Graphics& g)
{
}

void LabelAndTimeWindow::resized() 
{
    int width = getLocalBounds().getWidth();
    int height = getLocalBounds().getHeight();

    int margin = 10;

    int col = (width - (2*margin))/4;
    int line = (height - (2*margin))/8;
    nameLabel.setBounds(margin + 0.5 * col,margin+ 0, 3 * col, line);
    htpUpLabel.setBounds(margin + col, margin + 3 * line, col, line);
    htpDownLabel.setBounds(margin + 2 * col, margin + 3 * line, col, line);
    ltpLabel.setBounds(margin + 3 * col, margin + 3 * line, col, line);
    delayLabel.setBounds(margin + 0, margin + 4 * line, col, line);
    fadeLabel.setBounds(margin + 0, margin + 5 * line, col, line);
    nameEdit.setBounds(margin + 0.5 * col, margin + line, 3 * col, line);
    htpUpDelayEdit.setBounds(margin + 1 * col, margin + 4 * line, col, line);
    htpDownDelayEdit.setBounds(margin + 2 * col, margin + 4 * line, col, line);
    ltpDelayEdit.setBounds(margin + 3 * col, margin + 4 * line, col, line);
    htpUpFadeEdit.setBounds(margin + 1 * col, margin + 5 * line, col, line);
    htpDownFadeEdit.setBounds(margin + 2 * col, margin + 5 * line, col, line);
    ltpFadeEdit.setBounds(margin + 3 * col, margin + 5 * line, col, line);
    validBtn.setBounds(margin + 0.5 * col, margin + 7 * line, col, line);
    exitBtn.setBounds(margin + 2.5 * col, margin + 7 * line, col, line);
}

void LabelAndTimeWindow::applyValues()
{
    BKEngine* e = dynamic_cast<BKEngine*>(BKEngine::mainEngine);
    bool multiple = e->selectedCues.size() > 1;

    String nameString = nameEdit.getText();
    String inDelayString = htpUpDelayEdit.getText().trim();
    String outDelayString = htpDownDelayEdit.getText().trim();
    String ltpDelayString = ltpDelayEdit.getText().trim();
    String inFadeString = htpUpFadeEdit.getText().trim();
    String outFadeString = htpDownFadeEdit.getText().trim();
    String ltpFadeString = ltpFadeEdit.getText().trim();

    for (int i = 0; i < e->selectedCues.size(); i++) {
        Cue* c = e->selectedCues[i];
        if (nameString.trim() != "") {
            
            String name = multiple ? nameString + " " + String(i + 1) : nameString;
            c->setNiceName(name);
        }
        
        if (inDelayString != "") {
            float inDelay = c->htpInDelay->floatValue();
            if (newValue(inDelayString, inDelay)) {
                c->htpInDelay->setValue(inDelay);
            }
        }
        if (outDelayString != "") {
            float outDelay = c->htpOutDelay->floatValue();
            if (newValue(outDelayString, outDelay)) {
                c->htpOutDelay->setValue(outDelay);
            }
        }
        if (ltpDelayString != "") {
            float ltpDelay = c->ltpDelay->floatValue();
            if (newValue(ltpDelayString, ltpDelay)) {
                c->ltpDelay->setValue(ltpDelay);
            }
        }

        if (inFadeString != "") {
            float inFade = c->htpInFade->floatValue();
            if (newValue(inFadeString, inFade)) {
                c->htpInFade->setValue(inFade);
            }
        }
        if (outFadeString != "") {
            float outFade = c->htpOutFade->floatValue();
            if (newValue(outFadeString, outFade)) {
                c->htpOutFade->setValue(outFade);
            }
        }
        if (ltpFadeString != "") {
            float ltpFade = c->ltpFade->floatValue();
            if (newValue(ltpFadeString, ltpFade)) {
                c->ltpFade->setValue(ltpFade);
            }
        }

    }

}

bool LabelAndTimeWindow::newValue(String input, float& currentVal)
{
    if (String("+-*/").contains(input.substring(0, 1))) {
        input = String(currentVal)+input;
    }
    String error;
    Expression exp(input, error);
    if (error == "") {
        double v = exp.evaluate();
        currentVal = v;
    }
    return true;
}

void LabelAndTimeWindow::showWindow(int element)
{
    double now = Time::getMillisecondCounterHiRes();
    if (UserInputManager::getInstance()->lastLabelAndTimeWindowTS + 500 < now) {
        UserInputManager::getInstance()->lastLabelAndTimeWindowTS = now;

        BKEngine* e = dynamic_cast<BKEngine*>(Engine::mainEngine);
        int w = 400;
        int h = 200;

        closeWindow();
        setSize(w, h);
        resized();
        DialogWindow::showDialog("Label and time", this, &ShapeShifterManager::getInstance()->mainContainer, Colours::black, true);
        if (posX != 0 && posY != 0) {
            findParentComponentOfClass<DialogWindow>()->setBounds(posX, posY, w, h);
        }

        switch (element) {
        case 0:        nameEdit.grabKeyboardFocus();    break;
        case 1:        htpUpDelayEdit.grabKeyboardFocus();    break;
        case 2:        htpDownDelayEdit.grabKeyboardFocus();    break;
        case 3:        ltpDelayEdit.grabKeyboardFocus();    break;
        case 4:        htpUpFadeEdit.grabKeyboardFocus();    break;
        case 5:        htpDownFadeEdit.grabKeyboardFocus();    break;
        case 6:        ltpFadeEdit.grabKeyboardFocus();    break;

        }
    }
}

void LabelAndTimeWindow::buttonClicked(Button* b)
{
    if (b == &validBtn) {
        applyValues();
    }
    auto test = findParentComponentOfClass<DialogWindow>()->getBounds();
    posX = test.getX();
    posY = test.getY();
    closeWindow();
    nameEdit.setText("", juce::dontSendNotification);
    htpUpDelayEdit.setText("", juce::dontSendNotification);
    htpDownDelayEdit.setText("", juce::dontSendNotification);
    ltpDelayEdit.setText("", juce::dontSendNotification);
    htpUpFadeEdit.setText("", juce::dontSendNotification);
    htpDownFadeEdit.setText("", juce::dontSendNotification);
    ltpFadeEdit.setText("", juce::dontSendNotification);
}

void LabelAndTimeWindow::closeWindow() {
    DialogWindow* dw = findParentComponentOfClass<DialogWindow>();
    if (dw != nullptr) {
        dw->unfocusAllComponents();
        dw->exitModalState(0);
    }
}
