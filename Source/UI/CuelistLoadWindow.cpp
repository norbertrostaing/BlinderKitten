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
    addAndMakeVisible(viewport);
    viewport.setViewedComponent(&btnContainer);
    viewport.setBounds(0, 0, 810, 610);
    btnContainer.setSize(800, 200);

}

CuelistLoadWindow::~CuelistLoadWindow()
{
}

void CuelistLoadWindow::paint (juce::Graphics& g)
{
}

void CuelistLoadWindow::resized()
{
    int x = 10;
    int y = -1; 
    int w = getWidth()/5;
    int h = 40;

    if (cueIds.size() == 0) {return;}

    float prevId = cueIds[0];
    for (int i = 0; i < buttons.size(); i++) {
        float id = cueIds[i];
        x++;
        if (x >= 5 || prevId + 1 < id) {
            y++;
            x = 0;
        }
        TextButton* b = buttons[i];
        b->setBounds(x*w, y*h, w, h);
        b->addListener(this);
        prevId = id;
    }
    btnContainer.setSize(800, (y+1)*h);

}

void CuelistLoadWindow::fillButtons(Cuelist* c) {
    currentTarget = c;
    // bug ici ! quand load une cuelist, rajouter un lock ?
    buttons.clear();
    cueIds.clear();
    for (int i = 0; i < c->cues.items.size(); i++) {
        TextButton *temp = new TextButton();
        temp->setButtonText(c->cues.items[i]->niceName);
        if (c->cueA == c->cues.items[i]) {
            temp->setColour(TextButton::buttonColourId, Colour(128, 128, 128));
            temp->setColour(TextButton::textColourOffId, Colour(255,255,255));
        }
        btnContainer.addAndMakeVisible(temp);
        buttons.add(temp);
        cueIds.add(c->cues.items[i]->id->floatValue());
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
            currentTarget->userGo();
        }
    }
    closeWindow();
}

void CuelistLoadWindow::closeWindow() {
    findParentComponentOfClass<DialogWindow>()->unfocusAllComponents();
    findParentComponentOfClass<DialogWindow>()->exitModalState(0);
}