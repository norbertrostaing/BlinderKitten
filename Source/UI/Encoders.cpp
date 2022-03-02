/*
  ==============================================================================

    Encoders.cpp
    Created: 19 Feb 2022 12:19:42am
    Author:  No

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Encoders.h"
#include "UserInputManager.h"
#include "Definitions/ChannelFamily/ChannelType/ChannelType.h"
#include "Definitions/Programmer/Programmer.h"
#include "Definitions/Command/Command.h"

//==============================================================================
EncodersUI::EncodersUI(const String& contentName):
    ShapeShifterContent(Encoders::getInstance(), contentName)
{
}

EncodersUI::~EncodersUI()
{
}

juce_ImplementSingleton(Encoders);

Encoders::Encoders():
    channels()
{
    addAndMakeVisible(&commandLine);

    addAndMakeVisible(&btnMode);
    btnMode.addListener(this);
    btnMode.setButtonText("Val");

    addAndMakeVisible(&bigMoveLeftBtn);
    bigMoveLeftBtn.addListener(this);
    bigMoveLeftBtn.setButtonText("<<");

    addAndMakeVisible(&littleMoveLeftBtn);
    littleMoveLeftBtn.addListener(this);
    littleMoveLeftBtn.setButtonText("<");

    addAndMakeVisible(&bigMoveRightBtn);
    bigMoveRightBtn.addListener(this);
    bigMoveRightBtn.setButtonText(">>");

    addAndMakeVisible(&littleMoveRightBtn);
    littleMoveRightBtn.addListener(this);
    littleMoveRightBtn.setButtonText(">");

    for (int i = 0; i < nEncoders; i++) {
        Slider* s = new Slider();
        addAndMakeVisible(s);
        s->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        s->setRange(0,1);
        s->setValue(0,juce::dontSendNotification);
        s->setColour(Slider::rotarySliderFillColourId, Colour(63,63,63));
        s->setNumDecimalPlacesToDisplay(5);
        s->addListener(this);
        encoders.add(s);

        Label* l = new Label();
        addAndMakeVisible(l);
        l->setText("", juce::dontSendNotification);
        l->attachToComponent(s, false);
        labels.add(l);
    }

}

Encoders::~Encoders()
{
}

void Encoders::paint (juce::Graphics& g)
{
    
}

void Encoders::resized()
{

    float scale = 1;
    if (engine != nullptr && engine->encodersScale != nullptr) {
        scale = engine->encodersScale->getValue();
    }
    int nChildren = getNumChildComponents();
    for (int i = 0; i < nChildren; i++) {
        getChildComponent(i)->setTransform(AffineTransform::scale(scale));

    }

    // This method is where you should set the bounds of any child
    // components that your component contains..
    int windowH = getHeight();
    int windowW = getWidth();
    int x = 0;
    int y = 0;
    int btnWidth = 30;
    int btnValueWidth = 40;
   
    commandLine.setBounds(0,22,windowW, 20);

    btnMode.setBounds(windowW - btnValueWidth, 0, 40, 20);
    bigMoveRightBtn.setBounds(windowW - btnValueWidth-(1*btnWidth), 0, btnWidth, 20);
    littleMoveRightBtn.setBounds(windowW - btnValueWidth - (2 * btnWidth), 0, btnWidth, 20);
    littleMoveLeftBtn.setBounds(windowW - btnValueWidth - (3 * btnWidth), 0, btnWidth, 20);
    bigMoveLeftBtn.setBounds(windowW - btnValueWidth - (4 * btnWidth),0, btnWidth,20);

    if (windowH > windowW) { // portrait
        float w = 120;
        float h = 60;
        for (int i = 0; i < 10; i++) {
            encoders[i]->setBounds(0, i*h, w, h);
            encoders[i]->setTextBoxStyle(Slider::TextBoxRight, false, 44, 20);
            labels[i]->setBounds(60, i * h, 60, 20);
            labels[i]->setJustificationType(1);

        }
    }
    else {
        float w = 60;
        float h = 60;
        for (int i = 0; i < 10; i++) {
            int encoderId = i + encodersOffset;
            encoders[i]->setBounds(i*w, 60, w, h);
            encoders[i]->setTextBoxStyle(Slider::TextBoxBelow, false, 44, 20);
            labels[i]->setBounds(i*w, 80, 60, 20);
            labels[i]->setJustificationType(36);
        }
    }
}

void Encoders::sliderValueChanged(Slider* slider)
{   
    int index = encoders.indexOf(slider)+encodersOffset;
    UserInputManager::getInstance()->encoderValueChanged(index, slider->getValue());
    
}

void Encoders::buttonClicked(Button* b) {
    if (b == &btnMode) {
        mode = (mode+1) % 2;
        updateModeButton();
    }
    else if (b == &bigMoveLeftBtn) {
        int bigOffset = engine->encoderBigNumber->getValue();
        encodersOffset = jmax(0, encodersOffset - bigOffset);
        updateEncoders();
    }
    else if (b == &bigMoveRightBtn) {
        int bigOffset = engine->encoderBigNumber->getValue();
        encodersOffset = jmax(0, encodersOffset + bigOffset);
        updateEncoders();
    }
    else if (b == &littleMoveLeftBtn) {
        encodersOffset = jmax(0, encodersOffset - 1);
        updateEncoders();
    }
    else if (b == &littleMoveRightBtn) {
        encodersOffset = jmax(0, encodersOffset + 1);
        updateEncoders();
    }
}

void Encoders::updateModeButton() {
    if (mode == 0) {
        btnMode.setButtonText("Val");
    }
    else if (mode == 1) {
        btnMode.setButtonText("Thru");
    }
    else if (mode == 2) {
        btnMode.setButtonText("Time");
    }
    updateEncoders();
}

void Encoders::updateFilters() {
    // add buttons to filter types of channels by family
}

void Encoders::updateEncoders() {
    Command * currentCommand = nullptr; 
    if (UserInputManager::getInstance()->currentProgrammer != nullptr) {
        currentCommand = UserInputManager::getInstance()->currentProgrammer->currentUserCommand;
    }
    encodersOffset = jmin(encodersOffset, channels.size()-nEncoders);
    encodersOffset = jmax(encodersOffset, 0);

    for (int i = 0; i < nEncoders; i++) {
        int channelId = i+encodersOffset;
        if (mode == 2) {
            String l = "";
            if (i == 0) { l = "Delay From"; }
            if (i == 1) { l = "Delay To"; }
            if (i == 2) { l = "Fade From"; }
            if (i == 3) { l = "Fade to "; }
            labels[i]->setText(l, juce::sendNotification);
        }
        else if (channels.size() > channelId) {
            labels[i]->setText(String(channels[channelId]->niceName), juce::sendNotification);
            encoders[i]->setEnabled(true);
            encoders[i]->setColour(Slider::rotarySliderFillColourId, Colour(192, 192, 192));
            encoders[i]->setValue(0, juce::dontSendNotification);
            if (currentCommand != nullptr) {
                float v = currentCommand->getChannelValue(channels[channelId], mode == 1);
                if (v >= 0) {
                    encoders[i]->setColour(Slider::rotarySliderFillColourId, Colour(255, 0, 0));
                    encoders[i]->setValue(v, juce::dontSendNotification);
                }
            }
        }
        else {
            labels[i]->setText("", juce::dontSendNotification);
            encoders[i]->setEnabled(false);
            encoders[i]->setColour(Slider::rotarySliderFillColourId, Colour(63, 63, 63));
            encoders[i]->setValue(0, juce::dontSendNotification);
        }
        labels[i]->repaint();
    }
    resized();

}

void Encoders::updateContentWithCommand(Command* c) {
    for (int i = 0; i < c->values.items.size(); i++) {
        CommandValue* cv = c->values.items[i];
        if (cv->presetOrValue->getValue() == "value") {
            ChannelType* ct = dynamic_cast<ChannelType*>(cv->channelType->targetContainer.get());
            if (ct != nullptr) {
                for (int ci = 0; ci < channels.size(); ci++) {
                    int channelId = ci + encodersOffset;
                    if (channels[channelId] == ct) {
                        float v = c->getChannelValue(channels[channelId], mode == 1);
                        Encoders::getInstance()->encoders[channelId]->setValue(v, juce::sendNotification);
                        Encoders::getInstance()->encoders[channelId]->setColour(Slider::rotarySliderFillColourId, Colour(255, 0, 0));
                    }
                }
            }
        }
    }

}