/*
  ==============================================================================

	DMXChannelView.cpp
	Created: 8 Oct 2020 6:59:51pm
	Author:  bkupe

  ==============================================================================
*/

#include "CuelistSheetLine.h"
#include "BKEngine.h"
#include "CuelistSheet.h"
#include "UserInputManager.h"
#include "Definitions/Cuelist/Cuelist.h"


CuelistSheetLine::CuelistSheetLine(Cue* c, CuelistSheet* parent)
{
	//setRepaintsOnMouseActivity(true);
	targetCue = c;
	parentCuelistSheet = parent;
	targetCue->addChangeListener(this);

	addMouseListener(this, true);

	addAndMakeVisible(cueIdLabel);
	addAndMakeVisible(cueNameLabel);
	addAndMakeVisible(releaseTrackingButton);

	addAndMakeVisible(htpUpDelayLabel);
	addAndMakeVisible(htpDownDelayLabel);
	addAndMakeVisible(ltpDelayLabel);

	addAndMakeVisible(htpUpFadeLabel);
	addAndMakeVisible(htpDownFadeLabel);
	addAndMakeVisible(ltpFadeLabel);

	addAndMakeVisible(actionsBtn);
	actionsBtn.setButtonText("...");
	actionsBtn.setWantsKeyboardFocus(false);

	actionsBtn.onClick = [this](){
		showCuePopup();
	};


	cueIdLabel.setEditable(false, true, false);
	cueIdLabel.addListener(this);
	cueNameLabel.setEditable(false, true, false);
	cueNameLabel.addListener(this);
	htpUpDelayLabel.setEditable(false, true, false);
	htpUpDelayLabel.addListener(this);
	htpDownDelayLabel.setEditable(false, true, false);
	htpDownDelayLabel.addListener(this);
	ltpDelayLabel.setEditable(false, true, false);
	ltpDelayLabel.addListener(this);
	htpUpFadeLabel.setEditable(false, true, false);
	htpUpFadeLabel.addListener(this);
	htpDownFadeLabel.setEditable(false, true, false);
	htpDownFadeLabel.addListener(this);
	ltpFadeLabel.setEditable(false, true, false);
	ltpFadeLabel.addListener(this);
	releaseTrackingButton.addListener(this);

	cueIdLabel.setJustificationType(Justification::right);
	cueNameLabel.setJustificationType(Justification::left);
	htpUpDelayLabel.setJustificationType(Justification::centred);
	htpDownDelayLabel.setJustificationType(Justification::centred);
	ltpDelayLabel.setJustificationType(Justification::centred);
	htpUpFadeLabel.setJustificationType(Justification::centred);
	htpDownFadeLabel.setJustificationType(Justification::centred);
	ltpFadeLabel.setJustificationType(Justification::centred);



	addAndMakeVisible(isSelected);
	isSelected.addListener(this);
	isSelected.setWantsKeyboardFocus(false);
}

CuelistSheetLine::~CuelistSheetLine()
{
	if (targetCue != nullptr) {
		targetCue->removeChangeListener(this);
	}
}



void CuelistSheetLine::paint(Graphics& g)
{
	if (isRunning || mouseOver ) {
		Colour c = isRunning ? Colour(80, 64, 24) : Colour(32,32,32);
		if (mouseOver) c = c.brighter();
		g.setColour(c);
		g.fillRect(getLocalBounds());
	}
}

void CuelistSheetLine::resized()
{
	int width = getWidth();
	int w = width/10.5;
	w = jmin(45, w);

	BKEngine* e = (BKEngine*)Engine::mainEngine;
	float textSize = e->cuelistSheetTextSize->floatValue();
	int h = textSize*2;

	int nameWidth = width - (8.5*w);

	Font f (textSize);

	isSelected.setBounds(0,0,0.5*w,h); 
	cueIdLabel.setBounds(0.5 * w, 0, w, h);
	cueNameLabel.setBounds(1.5 * w, 0, nameWidth, h);

	htpUpDelayLabel.setBounds(width - (7.5 * w), 0, w, h);
	htpDownDelayLabel.setBounds(width - (6.5 * w), 0, w, h);
	ltpDelayLabel.setBounds(width - (5.5 * w), 0, w, h);
	htpUpFadeLabel.setBounds(width - (4.5 * w), 0, w, h);
	htpDownFadeLabel.setBounds(width - (3.5 * w), 0, w, h);
	ltpFadeLabel.setBounds(width - (2.5 * w), 0, w, h);
	releaseTrackingButton.setBounds(width - (1.5 * w), 0, w/2, h);

	actionsBtn.setBounds(width - (1 * w), 0, 1 * w, h);

	cueIdLabel.setFont(f);
	cueNameLabel.setFont(f);

	htpUpDelayLabel.setFont(f);
	htpDownDelayLabel.setFont(f);
	ltpDelayLabel.setFont(f);
	htpUpFadeLabel.setFont(f);
	htpDownFadeLabel.setFont(f);
	ltpFadeLabel.setFont(f);

	finalHeight = h;
}

void CuelistSheetLine::updateContent()
{
	cueIdLabel.setText(String(targetCue->id->floatValue()), juce::dontSendNotification);
	cueNameLabel.setText(targetCue->niceName, juce::dontSendNotification);

	float htpInDelay = int(100 * targetCue->htpInDelay->floatValue())/100.;
	float htpOutDelay = int(100 * targetCue->htpOutDelay->floatValue()) / 100.;
	float ltpDelay = int(100 * targetCue->ltpDelay->floatValue()) / 100.;
	float htpInFade = int(100 * targetCue->htpInFade->floatValue()) / 100.;
	float htpOutFade = int(100 * targetCue->htpOutFade->floatValue()) / 100.;
	float ltpFade = int(100 * targetCue->ltpFade->floatValue()) / 100.;

	htpUpDelayLabel.setText(htpInDelay < 0 ? "-" : String(htpInDelay), juce::dontSendNotification);
	htpDownDelayLabel.setText(htpOutDelay < 0 ? "-" : String(htpOutDelay), juce::dontSendNotification);
	ltpDelayLabel.setText(ltpDelay < 0 ? "-" : String(ltpDelay), juce::dontSendNotification);
	htpUpFadeLabel.setText(htpInFade < 0 ? "-" : String(htpInFade), juce::dontSendNotification);
	htpDownFadeLabel.setText(htpOutFade < 0 ? "-" : String(htpOutFade), juce::dontSendNotification);
	ltpFadeLabel.setText(ltpFade < 0 ? "-" : String(ltpFade), juce::dontSendNotification); 
	releaseTrackingButton.setToggleState(targetCue->releaseCurrentTracking->boolValue(), juce::dontSendNotification);

}

void CuelistSheetLine::buttonClicked(Button* b)
{
	if (b == &isSelected) {
		ModifierKeys k = ModifierKeys::getCurrentModifiersRealtime();
		BKEngine::selectionMode m = BKEngine::SET;
		if (k.isCtrlDown()) m = BKEngine::ADD;
		if (k.isShiftDown()) m = BKEngine::ADDMULTIPLE;
		dynamic_cast<BKEngine*>(BKEngine::mainEngine)->selectCue(targetCue, m);
	}
	else if (b == &releaseTrackingButton) {
		targetCue->releaseCurrentTracking->setValue(b->getToggleState());
	}
}

void CuelistSheetLine::changeListenerCallback(ChangeBroadcaster* source)
{
	Cue* c = dynamic_cast<Cue*>(source);
	if (c != nullptr) {
		updateContent();
	}
}

void CuelistSheetLine::labelTextChanged(Label* l)
{
	if (targetCue == nullptr) return;
	if (l == &cueIdLabel) { targetCue->id->setValue(l->getText().getFloatValue()); }
	if (l == &cueNameLabel) { targetCue->setNiceName(l->getText()); }
	if (l == &htpUpDelayLabel) 
	{ 
		float val = l->getText().trim() == "" ? -1 : l->getText().getFloatValue();
		targetCue->htpInDelay->setValue(val);
	}
	if (l == &htpDownDelayLabel)
	{
		float val = l->getText().trim() == "" ? -1 : l->getText().getFloatValue();
		targetCue->htpOutDelay->setValue(val);
	}
	if (l == &ltpDelayLabel)
	{
		float val = l->getText().trim() == "" ? -1 : l->getText().getFloatValue();
		targetCue->ltpDelay->setValue(val);
	}
	if (l == &htpUpFadeLabel)
	{
		float val = l->getText().trim() == "" ? -1 : l->getText().getFloatValue();
		targetCue->htpInFade->setValue(val);
	}
	if (l == &htpDownFadeLabel)
	{
		float val = l->getText().trim() == "" ? -1 : l->getText().getFloatValue();
		targetCue->htpOutFade->setValue(val);
	}
	if (l == &ltpFadeLabel)
	{
		float val = l->getText().trim() == "" ? -1 : l->getText().getFloatValue();
		targetCue->ltpFade->setValue(val); 
	}
}

void CuelistSheetLine::showCuePopup()
{
	if (targetCue == nullptr) return;
	Cuelist* targetCuelist = CuelistSheet::getInstance()->targetCuelist;
	if (targetCuelist == nullptr) return;
	PopupMenu p;
	p.addItem("Inspect", [this, targetCuelist]() {targetCue->selectThis();});
	p.addItem("Load as next cue", [this, targetCuelist]() {targetCuelist->nextCue->setValueFromTarget(targetCue);});
	p.addItem("Load and go", [this, targetCuelist]() {targetCuelist->nextCue->setValueFromTarget(targetCue); targetCuelist->go();});
	p.addItem("Load and go instant", [this, targetCuelist]() {targetCuelist->nextCue->setValueFromTarget(targetCue); targetCuelist->go(0,0); });
	p.addSeparator();
	p.addItem("Load content",[this, targetCuelist](){targetCue->loadContent(UserInputManager::getInstance()->getProgrammer(true));});
	p.addSeparator();
	p.addItem("Insert before", [this, targetCuelist]() {targetCuelist->insertProgCueBefore(targetCue); });
	p.addItem("Insert after", [this, targetCuelist]() {targetCuelist->insertProgCueAfter(targetCue); });
	p.addSeparator();
	p.addItem("Delete", [this, targetCuelist]() {targetCue->remove(); });
	p.addSeparator();
	p.addItem("Replace", [this, targetCuelist]() {targetCue->replaceContent(UserInputManager::getInstance()->getProgrammer(true)); });
	p.addItem("Merge", [this, targetCuelist]() {targetCue->mergeContent(UserInputManager::getInstance()->getProgrammer(true)); });

	p.showMenuAsync(PopupMenu::Options(), [this](int result){});

}

void CuelistSheetLine::mouseEnter(const MouseEvent& event)
{
	mouseOver = true;
	repaint();
}

void CuelistSheetLine::mouseExit(const MouseEvent& event)
{
	mouseOver = false;
	repaint();
}

