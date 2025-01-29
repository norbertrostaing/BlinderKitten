#include <JuceHeader.h>
#include "CuelistSheet.h"
#include "Brain.h"
#include "UI/LabelAndTime.h"

//==============================================================================
CuelistSheetUI::CuelistSheetUI(const String& contentName):
    ShapeShifterContent(CuelistSheet::getInstance(), contentName)
{
    
}

CuelistSheetUI::~CuelistSheetUI()
{
}

juce_ImplementSingleton(CuelistSheet);

CuelistSheet::CuelistSheet()
{
    addAndMakeVisible(labelAndTimeBtn);
    addAndMakeVisible(inspectBtn);

    addAndMakeVisible(cueIdLabel); cueIdLabel.setText("ID", juce::dontSendNotification);
    addAndMakeVisible(cueNameLabel); cueNameLabel.setText("Name", juce::dontSendNotification);
    addAndMakeVisible(htpUpDelayLabel); htpUpDelayLabel.setText("D up", juce::dontSendNotification);
    addAndMakeVisible(htpDownDelayLabel); htpDownDelayLabel.setText("D Down", juce::dontSendNotification);
    addAndMakeVisible(ltpDelayLabel); ltpDelayLabel.setText("D LTP", juce::dontSendNotification);
    addAndMakeVisible(htpUpFadeLabel); htpUpFadeLabel.setText("F Up", juce::dontSendNotification);
    addAndMakeVisible(htpDownFadeLabel); htpDownFadeLabel.setText("F Down", juce::dontSendNotification);
    addAndMakeVisible(ltpFadeLabel); ltpFadeLabel.setText("F LTP", juce::dontSendNotification);
    addAndMakeVisible(releaseTrackingLabel); releaseTrackingLabel.setText("RT", juce::dontSendNotification);

    htpUpDelayLabel.setJustificationType(Justification::centred);
    htpDownDelayLabel.setJustificationType(Justification::centred);
    ltpDelayLabel.setJustificationType(Justification::centred);
    htpUpFadeLabel.setJustificationType(Justification::centred);
    htpDownFadeLabel.setJustificationType(Justification::centred);
    ltpFadeLabel.setJustificationType(Justification::centred);
    releaseTrackingLabel.setJustificationType(Justification::centred);

    addAndMakeVisible(isSelected);
    isSelected.onClick = [this](){
        BKEngine* e = dynamic_cast<BKEngine*>(BKEngine::mainEngine);
        if (isSelected.getToggleState()) {
            e->selectAllCuesOfCuelist(targetCuelist);
        }
        else {
            e->clearCueSelection();
        }
    };
    isSelected.setWantsKeyboardFocus(false);

    labelAndTimeBtn.onClick = [this]() {LabelAndTimeWindow::getInstance()->showWindow(); };
    inspectBtn.onClick = [this]() {
        PopupMenu p;
        p.addItem("Current cue", [this]() { inspect(0); });
        p.addSeparator();
        p.addItem("Previous cue", [this]() { inspect(-1); });
        p.addItem("Next cue", [this]() { inspect(1); });
        p.addSeparator();
        p.addItem("Cuelist", [this]() { inspectCuelist(); });
        p.addSeparator();
        p.addItem("Export Cuelist as HTML", [this]() { exportCuelist(); });
        p.showMenuAsync(PopupMenu::Options(), [this](int result) {});
        };


    labelAndTimeBtn.setButtonText("Edit Label and time");
    labelAndTimeBtn.setWantsKeyboardFocus(false);
    inspectBtn.setButtonText("Inspect");
    inspectBtn.setWantsKeyboardFocus(false);

    BKEngine* e = dynamic_cast<BKEngine*>(Engine::mainEngine);
    mainIdSlider = e->conductorCuelistId->createDefaultUI();
    mainIdSlider->showLabel = false;
    addAndMakeVisible(mainIdSlider);

    addAndMakeVisible(viewport);
    viewport.setViewedComponent(&linesContainer);
}

CuelistSheet::~CuelistSheet()
{
    if (targetCuelist != nullptr) targetCuelist->removeChangeListener(this);
    delete mainIdSlider;
}

void CuelistSheet::paint (juce::Graphics& g)
{
    if (engine == nullptr) { return; }
    if (engine != nullptr) { return; }
    g.fillAll(juce::Colours::black);   // clear the background

    int targetCueId = engine->conductorCuelistId->intValue();
    Cuelist* target = Brain::getInstance()->getCuelistById(targetCueId);
    if (target == nullptr) {
        g.drawText("Cuelist "+String(targetCueId)+" doesn't exist", getLocalBounds(), juce::Justification::centred, true);
        return;
    }

}

void CuelistSheet::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

    int width = getLocalBounds().getWidth();
    int height = getLocalBounds().getHeight();

    mainIdSlider->setBounds(width * 2 / 5, 0, width / 5, 20);
    labelAndTimeBtn.setBounds(width * 3 / 5, 0, width / 5, 20);
    inspectBtn.setBounds(width * 4 / 5, 0, width / 5, 20);

    int w = width / 10.5;
    w = jmin(45, w);
    BKEngine* e = (BKEngine*)Engine::mainEngine;
    float textSize = e->cuelistSheetTextSize->floatValue();
    int h = textSize * 2;
    int nameWidth = width - (8.5 * w);
    Font f(textSize);

    isSelected.setBounds(0, 20, 0.5 * w, h);
    cueIdLabel.setBounds(0.5 * w, 20, w, h);
    cueNameLabel.setBounds(1.5 * w, 20, nameWidth, h);

    htpUpDelayLabel.setBounds(width - (7.5 * w), 20, w, h);
    htpDownDelayLabel.setBounds(width - (6.5 * w), 20, w, h);
    ltpDelayLabel.setBounds(width - (5.5 * w), 20, w, h);
    htpUpFadeLabel.setBounds(width - (4.5 * w), 20, w, h);
    htpDownFadeLabel.setBounds(width - (3.5 * w), 20, w, h);
    ltpFadeLabel.setBounds(width - (2.5 * w), 20, w, h);
    releaseTrackingLabel.setBounds(width - (1.5 * w), 20, w/2, h);

    cueIdLabel.setFont(f);
    cueNameLabel.setFont(f);

    htpUpDelayLabel.setFont(f);
    htpDownDelayLabel.setFont(f);
    ltpDelayLabel.setFont(f);
    htpUpFadeLabel.setFont(f);
    htpDownFadeLabel.setFont(f);
    ltpFadeLabel.setFont(f);
    releaseTrackingLabel.setFont(f);

    viewport.setBounds(0,h+20,width, height-h-20);

    int currHeight = 0;
    for (int i = 0; i < lines.size(); i++) {
        lines[i]->resized();
        lines[i]->setBounds(0, currHeight, getWidth(), lines[i]->finalHeight);
        currHeight += lines[i]->finalHeight;
    }
    linesContainer.setBounds(0,0,getWidth() - 10, currHeight);
    

}

void CuelistSheet::updateStyle()
{
    Colour curr = engine->conductorCurrentCueColor->getColor();
    Colour next = engine->conductorNextCueColor->getColor();
}


void CuelistSheet::updateContent()
{
    resized();
    bool changed = false;
    Cuelist* newTarget = Brain::getInstance()->getCuelistById(engine->conductorCuelistId->intValue());
    if (targetCuelist != newTarget) {
        changed = true;
        if (targetCuelist != nullptr) targetCuelist->removeChangeListener(this);
        targetCuelist = newTarget;
        lines.clear();
    }
    if (targetCuelist == nullptr) return;

    if (changed) {
        targetCuelist->addChangeListener(this);
        rebuildLines();
    }
    else {
    }

}

void CuelistSheet::rebuildLines()
{
    lines.clear();
    if (targetCuelist == nullptr) return;
    for (int i = 0; i < targetCuelist->cues.items.size(); i++) {
        CuelistSheetLine* l = new CuelistSheetLine(targetCuelist->cues.items[i], this);
        lines.add(l);
        linesContainer.addAndMakeVisible(l);
        l->updateContent();
    }
    resized();
    updateSelection();
    updateRunningCue();
}

void CuelistSheet::inspect(int delta)
{
    int targetCueId = engine->conductorCuelistId->intValue();
    Cuelist* target = Brain::getInstance()->getCuelistById(targetCueId);
    if (target == nullptr) { return; }
    Cue* currentCue = target->cueA;
    int index = -1;
    if (currentCue != nullptr) {
        index = target->cues.items.indexOf(currentCue);
    }
    index += delta;
    if (index >= 0 && index < target->cues.items.size()) {
        target->cues.items[index]->selectThis();
    }
}

void CuelistSheet::inspectCuelist()
{
    if (targetCuelist == nullptr) { return; }
    targetCuelist->selectThis();
}

void CuelistSheet::exportCuelist()
{
    if (targetCuelist == nullptr) { return; }
    targetCuelist->exportInTextFile();
}

void CuelistSheet::updateSelection()
{
    BKEngine* e = dynamic_cast<BKEngine*>(BKEngine::mainEngine);
    for (int i = 0; i < lines.size(); i++) {
        lines[i]->isSelected.setToggleState(e->selectedCues.contains(lines[i]->targetCue), juce::dontSendNotification);
    }
}

void CuelistSheet::changeListenerCallback(ChangeBroadcaster* source)
{
    rebuildLines();
}

void CuelistSheet::cueDeleted(Cue* c)
{
    for (int i = 0; i < lines.size(); i++) {
        if (lines[i]->targetCue == c) lines[i]->targetCue = nullptr;
    }
    rebuildLines();
}

void CuelistSheet::updateRunningCue()
{
    if (targetCuelist == nullptr) return; 
    for (int i = 0; i < lines.size(); i++) {
        bool isRunning = targetCuelist->cueA == lines[i]->targetCue;
        if (lines[i]->isRunning != isRunning) 
        {
            lines[i]->isRunning = isRunning;
            lines[i]->repaint();
        }
        if (isRunning) {
            viewport.setViewPosition(0, lines[i]->getY() - viewport.getHeight()/2);
        }
    }
}

