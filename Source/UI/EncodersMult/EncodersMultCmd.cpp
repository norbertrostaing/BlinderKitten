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
    commandLineBtn.setButtonText("empty command");
    commandLineBtn.setBounds(0, 0, 50, 20);
    commandLineBtn.setToggleable(true);
    commandLineBtn.setToggleState(true, juce::dontSendNotification);
    commandLineBtn.setClickingTogglesState(true);
    addAndMakeVisible(&commandLineBtn);
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
        explodeBtn.setEnabled(targetCommand->selection.computedSelectedSubFixtures.size()>1);

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
                    f->showValue = true;
                    f->useCustomFGColor = true;
                    f->customFGColor = Colour(168,127,16);
                    addAndMakeVisible(f);
                    if (cv->thru->getValue()) {
                        FloatSliderUI* fThru = cv->valueTo->createSlider();
                        encoders.add(fThru);
                        fThru->orientation = FloatSliderUI::VERTICAL;
                        fThru->customLabel = ct->niceName+" thru";
                        fThru->showValue = true;
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
        commandLineBtn.setButtonText(targetCommand->getCommandSelectionAsTexts().joinIntoString(" "));
    }
    else {
        commandLineBtn.setButtonText("no cmd");
    }
}

void EncodersMultCmd::resized()
{

}

void EncodersMultCmd::calcSize()
{
    int w = 50;
    int h = 150;
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
    commandLineBtn.setSize(calculatedWidth, 20);
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

