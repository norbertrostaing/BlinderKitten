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
#include "BKEngine.h"

juce_ImplementSingleton(CuelistLoadWindow);


//==============================================================================
CuelistLoadWindow::CuelistLoadWindow()
{

}

CuelistLoadWindow::~CuelistLoadWindow()
{
}

void CuelistLoadWindow::paint (juce::Graphics& g)
{
}

void CuelistLoadWindow::resized()
{

    BKEngine* e = dynamic_cast<BKEngine*>(Engine::mainEngine);
    int nButtons = e->loadWindowButtonPerLine->intValue();
    int h = e->loadWindowButtonHeight->intValue();

    int x = 10;
    int y = -1; 
    int w = getWidth()/nButtons;

    if (cueIds.size() == 0) {return;}

    float prevId = cueIds[0];
    for (int i = 0; i < buttons.size(); i++) {
        float id = cueIds[i];
        x++;
        if (x >= nButtons || prevId + 1 < id) {
            y++;
            x = 0;
        }
        TextButton* b = buttons[i];
        b->setBounds(x*w, y*h, w, h);
        b->addListener(this);
        prevId = id;
    }
    btnContainer.setSize(getWidth()-10, (y + 1) * h);

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

void CuelistLoadWindow::showWindow()
{
    double now = Time::getMillisecondCounterHiRes();
    if (UserInputManager::getInstance()->lastCuelistLoadWindowTS + 500 < now && currentCuelist != nullptr) {
        UserInputManager::getInstance()->lastCuelistLoadWindowTS = now;

        BKEngine* e = dynamic_cast<BKEngine*>(Engine::mainEngine);
        int w = e->loadWindowWidth->intValue();
        int h = e->loadWindowHeight->intValue();

        setSize(w, h);
        addAndMakeVisible(viewport);
        viewport.setViewedComponent(&btnContainer);
        viewport.setBounds(0, 0, w, h);
        btnContainer.setSize(w-10, 200);

        closeWindow();
        fillButtons(currentCuelist);
        setSize(w, h);
        resized();
        DialogWindow::showDialog("Load next cue for " + currentCuelist->userName->getValue().toString(), this, &ShapeShifterManager::getInstance()->mainContainer, Colours::black, true);
        if (posX != 0 && posY != 0) {
            findParentComponentOfClass<DialogWindow>()->setBounds(posX, posY, w, h);
        }
    }
}


void CuelistLoadWindow::loadCuelist(Cuelist* c, bool triggerGoWhenSelected)
{   
    triggerGo = triggerGoWhenSelected;
    currentCuelist = c;
    MessageManager::callAsync([this](){showWindow();});
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
    auto test = findParentComponentOfClass<DialogWindow>()->getBounds();
    posX = test.getX();
    posY = test.getY();
    closeWindow();
}

void CuelistLoadWindow::closeWindow() {
    DialogWindow* dw = findParentComponentOfClass<DialogWindow>();
    if (dw != nullptr) {
        dw->unfocusAllComponents();
        dw->exitModalState(0);
    }
}
