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

//==============================================================================
EncodersUI::EncodersUI(const String& contentName):
    ShapeShifterContent(Encoders::getInstance(), contentName)
{
}

EncodersUI::~EncodersUI()
{
}

juce_ImplementSingleton(Encoders);

Encoders::Encoders()
{
    for (int i = 0; i < 8; i++) {
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
    // This method is where you should set the bounds of any child
    // components that your component contains..
    int windowH = getHeight();
    int windowsW = getWidth();
    int x = 0;
    int y = 0;
    if (windowH > windowsW) { // portrait
        float w = 120;
        float h = 60;
        for (int i = 0; i < 8; i++) {
            encoders[i]->setBounds(0, i*h, w, h);
            encoders[i]->setTextBoxStyle(Slider::TextBoxRight, false, 60, 20);
            labels[i]->setBounds(60, i * h, 60, 20);
            labels[i]->setJustificationType(1);

        }
    }
    else {
        float w = 80;
        float h = 80;
        for (int i = 0; i < 8; i++) {
            encoders[i]->setBounds(i*w, 0, w, h);
            encoders[i]->setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
            labels[i]->setBounds(i*w, 80, 80, 20);
            labels[i]->setJustificationType(36);
        }
    }
}

void Encoders::sliderValueChanged(Slider* slider)
{   
    int index = encoders.indexOf(slider);
    UserInputManager::getInstance()->encoderValueChanged(index, slider->getValue());
    
}

