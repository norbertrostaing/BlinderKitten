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
    test.setText("", juce::dontSendNotification);
    test.setBounds(0,0,200,20);
    addAndMakeVisible(test);
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

	cmdContainer.setBounds(Rectangle<int>(0, 0, r.getWidth() - 6, 400));
    int currentX = 0;
    int currentY = 0;
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
    cmdContainer.setBounds(0,0,r.getWidth(), currentY+highestY);
	viewport.setBounds(r);
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
        }

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
    commandItems.clear();
    cmdContainer.removeAllChildren();
}


