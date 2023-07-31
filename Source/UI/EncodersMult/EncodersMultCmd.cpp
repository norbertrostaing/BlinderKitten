/*
  ==============================================================================

	EncodersMult.cpp
	Created: 8 Oct 2020 6:59:51pm
	Author:  bkupe

  ==============================================================================
*/

#include "UserInputManager.h"
#include "Definitions/Programmer/Programmer.h"
#include "EncodersMultCmd.h"
#include "EncodersMult.h"
#include "UI/Encoders.h"
#include "Definitions/Command/Command.h"


EncodersMultCmd::EncodersMultCmd(EncodersMult* v, Command* c)
{
    encodersMulView = v;
    commandLine.setText("empty command", juce::dontSendNotification);
    commandLine.setBounds(0, 0, 50, 20);
    addAndMakeVisible(&commandLine);
    explodeBtn.setBounds(0, 20, 25, 20);
    explodeBtn.setButtonText("<>");
    explodeBtn.setHelpText("Explode command");
    explodeBtn.addListener(this);
    addAndMakeVisible(explodeBtn);
    deleteBtn.setBounds(25, 20, 25, 20);
    deleteBtn.setButtonText("X");
    deleteBtn.setHelpText("Delete command");
    deleteBtn.addListener(this);
    addAndMakeVisible(deleteBtn);
    resetCommand(c);
}

EncodersMultCmd::~EncodersMultCmd()
{
    if (targetCommand != nullptr) {
        //targetCommand->removeAsyncContainerListener(this);
    }
}

void EncodersMultCmd::resetCommand(Command* c)
{
    if (targetCommand != nullptr) {
        //targetCommand->removeAsyncContainerListener(this);
    }
    //c->addAsyncCoalescedContainerListener(this);
    targetCommand = c;
    commandContentChanged();
}

void EncodersMultCmd::commandContentChanged()
{
    const MessageManagerLock mmLock;
    channels.clear();
    for (int i = encoders.size(); i >= 0; i--) {
        encoders.remove(i); // maybe kill some things ?
    }
    if (targetCommand != nullptr) {
        for (int i = 0; i < targetCommand->values.items.size(); i++) {
            CommandValue *cv = targetCommand->values.items[i];
            if (cv->presetOrValue->getValue() == "value") {
                ChannelType* ct = dynamic_cast<ChannelType*>(cv->channelType->targetContainer.get());
                if (ct != nullptr) {
                    channels.add(ct);
                    FloatSliderUI* f = cv->valueFrom->createSlider();
                    encoders.add(f);
                    f->orientation = FloatSliderUI::VERTICAL;
                    f->customLabel = ct->niceName;
                    f->showValue = false;
                    f->useCustomFGColor = true;
                    f->customFGColor = Colour(168,127,16);
                    addAndMakeVisible(f);
                    if (cv->thru->getValue()) {
                        FloatSliderUI* fThru = cv->valueTo->createSlider();
                        encoders.add(fThru);
                        fThru->orientation = FloatSliderUI::VERTICAL;
                        fThru->customLabel = ct->niceName+" thru";
                        fThru->showValue = false;
                        fThru->useCustomFGColor = true;
                        fThru->customFGColor = Colour(168, 127, 16);
                        addAndMakeVisible(fThru);
                    }
                }
            }
        }
    }
    encodersMulView->resized();
    commandValueChanged();
}

void EncodersMultCmd::commandValueChanged()
{
    if (targetCommand != nullptr) {
        commandLine.setText(targetCommand->getCommandSelectionAsTexts().joinIntoString(" "), juce::dontSendNotification);
    }
    else {
        commandLine.setText("no cmd", juce::dontSendNotification);
    }
}

void EncodersMultCmd::resized()
{

}

void EncodersMultCmd::calcSize()
{
    int w = 50;
    int h = 100;
    int limitX = encodersMulView->viewport.getWidth();
    if (limitX == 0) {limitX = 300;} // to prevent bug at reload
    int maxX = 0;
    int currentX = 0;
    int currentY = 40;
    for (int i = 0; i < encoders.size(); i++) {
        if (currentX + w > limitX) {
            currentY += h;
            currentX = 0;
        }
        encoders[i]->setBounds(currentX, currentY, w, h);
        currentX += w;
        maxX = jmax(maxX, currentX);
    }
    calculatedHeight = currentY + h;
    calculatedWidth = maxX;
    commandLine.setSize(calculatedWidth, 20);
}

void EncodersMultCmd::sliderValueChanged(Slider* slider)
{
}

void EncodersMultCmd::newMessage(const ContainerAsyncEvent& e)
{
    auto t = e.targetControllable.get();
    if (t != nullptr) {
        if (t->niceName == "Channel type") {
            commandContentChanged();
        }
    }

}

void EncodersMultCmd::buttonClicked(Button* b)
{
    if (b == &explodeBtn) 
    {
        if (targetCommand != nullptr) {
            targetCommand->explodeSelection();
        }
    }
    else if (b == &deleteBtn) 
    {
        if (targetCommand != nullptr) {
            UserInputManager::getInstance()->currentProgrammer->checkCurrentUserCommand();
            if (targetCommand == UserInputManager::getInstance()->targetCommand) {
                if (UserInputManager::getInstance()->currentProgrammer != nullptr && UserInputManager::getInstance()->currentProgrammer->currentUserCommand == targetCommand) {
                    UserInputManager::getInstance()->currentProgrammer->selectNextCommand();
                }
                if (UserInputManager::getInstance()->currentProgrammer != nullptr && UserInputManager::getInstance()->currentProgrammer->currentUserCommand == targetCommand) {
                    UserInputManager::getInstance()->currentProgrammer->currentUserCommand = nullptr;
                    UserInputManager::getInstance()->targetCommand = nullptr;
                }
            }
            encodersMulView->targetCommandManager->removeItem(targetCommand);
            UserInputManager::getInstance()->currentProgrammer->checkCurrentUserCommand();
            Encoders::getInstance()->updateChannels();
        }

    }
}

