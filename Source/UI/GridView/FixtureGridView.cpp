/*
  ==============================================================================

    FixtureGridView.cpp
    Created: 19 Feb 2022 12:19:42am
    Author:  No

  ==============================================================================
*/

#include <JuceHeader.h>
#include "FixtureGridView.h"
#include "Brain.h"
#include "Definitions/Fixture/Fixture.h"
#include "Definitions/Fixture/FixtureManager.h"
#include "BKEngine.h"

//==============================================================================
FixtureGridViewUI::FixtureGridViewUI(const String& contentName) :
    ShapeShifterContent(FixtureGridView::getInstance(), contentName)
{

}

FixtureGridViewUI::~FixtureGridViewUI()
{
}

juce_ImplementSingleton(FixtureGridView);

FixtureGridView::FixtureGridView()
{
    targetType = "Fixture";
    completeGridMode = false;
    initArrays();
}

FixtureGridView::~FixtureGridView()
{
    //hashMapButtons.~HashMap();
}

void FixtureGridView::initArrays() {
    cs.enter();
    gridButtons.clear();
    hashMapButtons.clear();
    cs.exit();

}


void FixtureGridView::updateCells() {
    cs.enter();
    validCells.clear();
    gridButtons.clear();
    hashMapButtons.clear();
    fixtIdMax = 0;
    for (int i = 0; i < FixtureManager::getInstance()->items.size(); i++) {
        Fixture* f = FixtureManager::getInstance()->items[i];
        
        int id = f->id->getValue();
        fixtIdMax = jmax(id, fixtIdMax);

        TextButton* b = new TextButton();
        gridButtons.add(b);

        hashMapButtons.set(id, b);
        b->addListener(this);
        b->setButtonText(String(id)+" "+f->userName->stringValue());
        addAndMakeVisible(b);
    }
    cs.exit();
    resized();
}

void FixtureGridView::resized()
{
    float scale = 1;
    if (engine != nullptr && engine->gridScale != nullptr) {
        scale = engine->gridScale->getValue();
    }
    int h = getHeight();
    int w = getWidth();
    int scrollWidth = 10;
    int btnSizeH = 50 * scale;
    scroll.setBounds(w - scrollWidth, 0, scrollWidth, h);
    w = w - scrollWidth;

    int nByRow = w / btnSizeH;
    if (nByRow == 0) { return; }

    nByRow = engine->gridCols->getValue();

    int btnSizeW = w / nByRow;
    int margin = (w % btnSizeW) / 2;

    // int currentX = 0;
    int lastValidId = -5;
    

    OwnedArray<Array<TextButton*>> groupedButtons;
    for (int i = 0; i <= fixtIdMax; i++) 
    {
        if (hashMapButtons.contains(i)) 
        {
            TextButton* b = hashMapButtons.getReference(i);
            if (i > lastValidId + 1) {
                groupedButtons.add(new Array<TextButton*>());
            }
            groupedButtons.getLast()->add(b);
            lastValidId = i;
        }

    }

    int x = 0;
    int y = 0;
    int maxY = 0;

    for (int g = 0; g < groupedButtons.size(); g++) {
        Array<TextButton*>* group = groupedButtons[g];
        if (x != 0 && group->size() + x + 1 <= nByRow) {
            x++;
        }
        else {
            if (x != 0)  y += 1;
            x = 0;
        }
        for (int i = 0; i < group->size(); i++) {
            TextButton* b = group->getReference(i);
            int drawX = margin + (x * btnSizeW);
            int drawY = y * btnSizeH - scroll.getCurrentRangeStart();
            b->setBounds(drawX, drawY, btnSizeW, btnSizeH);
            maxY = (y + 1) * btnSizeH;
            x = (x + 1) % nByRow;
            if (x == 0) { y += 1; }
            lastValidId = i;

        }
    }

    scroll.setCurrentRange(scroll.getCurrentRangeStart(), h, juce::dontSendNotification);
    scroll.setRangeLimits(0, maxY, juce::dontSendNotification);
}

void FixtureGridView::buttonClicked(juce::Button* button) {
    cs.enter();
    for (auto it = hashMapButtons.begin(); it != hashMapButtons.end(); it.next()) {
        if (it.getValue() == button) {
            cellClicked(it.getKey());
            cs.exit();
            return;
        }
    }
    cs.exit();
}
