/*
  ==============================================================================

    CuelistLoadWindow.cpp
    Created: 4 Mar 2022 1:15:59pm
    Author:  No

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CuelistLoadWindow.h"

#include "Definitions//Cuelist/Cuelist.h"
#include "UserInputManager.h"

juce_ImplementSingleton(CuelistLoadWindow);


//==============================================================================
CuelistLoadWindow::CuelistLoadWindow()
{
    setSize(810, 610);
}

CuelistLoadWindow::~CuelistLoadWindow()
{
}

void CuelistLoadWindow::paint (juce::Graphics& g)
{
}

void CuelistLoadWindow::resized()
{
    int x = 0;
    int y = 0; 
    int w = getWidth()/5;
    int h = 40;

    for (int i = 0; i < buttons.size(); i++) {
        TextButton* b = buttons[i];
        b->setBounds(x*w, y*h, w, h);
        b->addListener(this);
        x++; 
        if (x >= 5) {
            y++;
            x = 0;
        }
    }
}

void CuelistLoadWindow::fillButtons(Cuelist* c) {
    currentTarget = c;
    // bug ici ! quand load une cuelist, rajouter un lock ?
    buttons.clear();
    for (int i = 0; i < c->cues.items.size(); i++) {
        TextButton *temp = new TextButton();
        temp->setButtonText(c->cues.items[i]->niceName);
        addAndMakeVisible(temp);
        buttons.add(temp);
    }
}

void CuelistLoadWindow::loadCuelist(Cuelist* c, bool triggerGoWhenSelected)
{   
    double now = Time::getMillisecondCounterHiRes();
    if (UserInputManager::getInstance()->lastCuelistLoadWindowTS + 500 < now) {
        UserInputManager::getInstance()->lastCuelistLoadWindowTS = now;
        triggerGo = triggerGoWhenSelected;
        closeWindow();
        fillButtons(c);
        setSize(810, 610);
        resized();
        DialogWindow::showDialog("Load next cue for " + c->userName->getValue().toString(), this, &ShapeShifterManager::getInstance()->mainContainer, Colours::black, true);
    }
}

void CuelistLoadWindow::buttonClicked(Button* b)
{
    int index = buttons.indexOf((TextButton*)b);
    if (currentTarget != nullptr && currentTarget->cues.items.size()>index) {
        currentTarget->nextCue->setValueFromTarget(currentTarget->cues.items[index]);
        currentTarget->fillTexts();
        if (triggerGo) {
            currentTarget->go();
        }
    }
    closeWindow();
}

void CuelistLoadWindow::closeWindow() {
    findParentComponentOfClass<DialogWindow>()->unfocusAllComponents();
    findParentComponentOfClass<DialogWindow>()->exitModalState(0);
}