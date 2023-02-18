/*
  ==============================================================================

	EncodersMult.cpp
	Created: 8 Oct 2020 6:59:51pm
	Author:  bkupe

  ==============================================================================
*/

#include "EncodersMult.h"
#include "UserInputManager.h"
#include "Definitions/Programmer/Programmer.h"
#include "EncodersMultCmd.h"

EncodersMult::EncodersMult() 
	//testingUI(nullptr),
{
	addAndMakeVisible(viewport);
	viewport.setViewedComponent(&cmdContainer);
    viewport.setBounds(0,20,100,100);
    cmdContainer.setSize(200,200);
}

juce_ImplementSingleton(EncodersMult);

EncodersMult::~EncodersMult()
{
    if (targetCommandManager != nullptr) {
        targetCommandManager->removeAsyncManagerListener(this);
    }
    //commandItems.clear();
}

void EncodersMult::newMessage(const CommandManager::ManagerEvent& e)
{
    reconstructSubComponents();
}

void EncodersMult::resized()
{
	Rectangle<int> r = getLocalBounds();
	Rectangle<int> hr = r.removeFromTop(20);
    int maxWidth = r.getWidth();
    float w = 57;
    float h = 57;
    int currentX = 0;
    int currentY = 0;

    for (int i = 0; i < encoders.size(); i++) {
        if (currentX + w > maxWidth) {
            currentY += 80; 
            currentX = 0;
        }
        encoders[i]->setBounds(currentX, currentY + 20, w, h);
        encoders[i]->setTextBoxStyle(Slider::TextBoxBelow, false, 44, 20);
        labels[i]->setBounds(currentX, currentY, w, 20);
        labels[i]->setJustificationType(36);
        currentX += w;
    }

    viewport.setBounds(0,currentY+80, r.getWidth(), r.getHeight()-currentY);

    currentX = 0;
    currentY = 0;
    int highestY = 0;
    for (int i = 0; i < commandItems.size(); i++) {
        commandItems[i]->calcSize();
        if (currentX + commandItems[i]->calculatedWidth > getWidth()) {
            currentY += highestY;
            highestY = 0;
            currentX = 0;
        }
        commandItems[i]->setBounds(currentX, currentY, commandItems[i]->calculatedWidth, commandItems[i]->calculatedHeight);
        highestY = jmax(highestY, commandItems[i]->calculatedHeight);
        currentX += commandItems[i]->calculatedWidth+10;
    }
    cmdContainer.setBounds(0,0,r.getWidth(), currentY+highestY+80);
}

void EncodersMult::targetChanged()
{
    if (targetCommandManager != nullptr) {
        //targetCommandManager->removeAsyncManagerListener(this);
    }
    Programmer* prog = UserInputManager::getInstance()->getProgrammer(true);
    targetCommandManager = &prog->commands;
    targetCommandManager->addAsyncManagerListener(this);
    reconstructSubComponents();
}

void EncodersMult::reconstructSubComponents()
{
    //test.setText("", juce::dontSendNotification);
    clear();
    if (targetCommandManager != nullptr) {
        for (int i = 0; i < targetCommandManager->items.size(); i++) {
            auto elmt = targetCommandManager->items[i];
            EncodersMultCmd* test = commandItems.add(new EncodersMultCmd(this, elmt));
            test->setBounds(10, i*50, 100, 50);
            cmdContainer.addAndMakeVisible(test);
            for (int n = 0; n < test->channels.size(); n++) {
                channels.addIfNotAlreadyThere(test->channels[n]);
            }
        }
    }
    for (int i = 0; i < channels.size(); i++) {
        Slider* s = new Slider();
        addAndMakeVisible(s);
        s->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        s->setRange(-1, 1);
        s->setValue(0, juce::dontSendNotification);
        s->setColour(Slider::rotarySliderFillColourId, Colour(127, 0, 0));
        s->setNumDecimalPlacesToDisplay(5);
        s->addListener(this);
        s->setWantsKeyboardFocus(false);
        s->addMouseListener(this, false);
        encoders.add(s);

        Label* l = new Label();
        addAndMakeVisible(l);
        l->setText(channels[i]->niceName, juce::dontSendNotification);
        l->attachToComponent(s, false);
        labels.add(l);
        l->setWantsKeyboardFocus(false);

        lastValues.add(0);

    }
    resized();
}

void EncodersMult::reconstructNames()
{
    for (int i = 0; i < commandItems.size(); i++) 
    {
    commandItems[i]->commandValueChanged();
    }
}

void EncodersMult::mouseDown(const MouseEvent& event)
{
}

void EncodersMult::clear()
{
    encoders.clear();
    labels.clear();
    channels.clear();
    lastValues.clear();
    commandItems.clear();
    cmdContainer.removeAllChildren();
}

void EncodersMult::sliderValueChanged(Slider* slider)
{
    int index = encoders.indexOf(slider);
    if (index != -1) {
        float val = slider->getValue();
        double delta = val- lastValues[index];
        lastValues.set(index, val);
        ChannelType* ct = channels[index];
        for (int i = 0; i < targetCommandManager->items.size(); i++) {
            for (int v = 0; v < targetCommandManager->items[i]->values.items.size(); v++) {
                CommandValue* cv = targetCommandManager->items[i]->values.items[v];
                if (cv->presetOrValue->stringValue() == "value") {
                    ChannelType* localCt = dynamic_cast<ChannelType*>(cv->channelType->targetContainer.get());
                    if (localCt == ct) {
                        cv->valueFrom->setValue(cv->valueFrom->floatValue() + delta);
                        if (cv->thru->boolValue()) {
                            cv->valueTo->setValue(cv->valueTo->floatValue() + delta);
                        }
                    }
                }
            }
        }
    }
}

void EncodersMult::sliderDragEnded(Slider* slider)
{
    int index = encoders.indexOf(slider);
    if (index != -1) {
        lastValues.set(index, 0);
        slider->setValue(0, juce::dontSendNotification);
    }
}


