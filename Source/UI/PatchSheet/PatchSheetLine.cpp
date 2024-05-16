/*
  ==============================================================================

	DMXChannelView.cpp
	Created: 8 Oct 2020 6:59:51pm
	Author:  bkupe

  ==============================================================================
*/

#include "PatchSheetLine.h"
#include "BKEngine.h"
#include "PatchSheet.h"
#include "UserInputManager.h"

PatchSheetLine::PatchSheetLine(Fixture* f, FixturePatch* fp, PatchSheet* parent)
{
	parentPatchSheet = parent;
	targetFixture = f;
	targetFixturePatch = fp;

	addMouseListener(this, true);

	if (f->patchs.items.indexOf(fp) == 0) {
		idControler = f->id->createLabelUI();
		idControler->showLabel = false;
		nameControler = f->userName->createStringParameterUI();
		nameControler->showLabel = false;
		typeControler = f->devTypeParam->createTargetUI();
		typeControler->showLabel = false;
		addAndMakeVisible(idControler);
		addAndMakeVisible(nameControler);
		addAndMakeVisible(typeControler);
	}
	if (fp != nullptr) {
		universeControler = fp->targetInterface->createTargetUI();
		universeControler-> showLabel = false;
		adressControler = fp->address->createLabelUI();
		adressControler->showLabel = false;
		enabledControler = fp->enabled->createButtonToggle();
		enabledControler->showLabel = false;
		addAndMakeVisible(universeControler);
		addAndMakeVisible(adressControler);
		addAndMakeVisible(enabledControler);
	}


	addAndMakeVisible(actionsBtn);
	actionsBtn.setButtonText("...");

	actionsBtn.onClick = [this](){
		showLinePopup();
	};

	//addAndMakeVisible(isSelected);
	//isSelected.addListener(this);
	//isSelected.setWantsKeyboardFocus(false);
}

PatchSheetLine::~PatchSheetLine()
{
	if (idControler != nullptr) {
		delete idControler; 
		idControler = nullptr;
	}
	if (nameControler != nullptr) {
		delete nameControler;
		nameControler = nullptr;
	}
	if (typeControler != nullptr) {
		delete typeControler;
		typeControler = nullptr;
	}
	if (universeControler != nullptr) {
		delete universeControler;
		universeControler = nullptr;
	}
	if (adressControler != nullptr) {
		delete adressControler;
		adressControler = nullptr;
	}
	if (enabledControler != nullptr) {
		delete enabledControler;
		enabledControler = nullptr;
	}


	
}



void PatchSheetLine::paint(Graphics& g)
{
	if (isRunning || mouseOver ) {
		Colour c = isRunning ? Colour(80, 64, 24) : Colour(32,32,32);
		if (mouseOver) c = c.brighter();
		g.setColour(c);
		g.fillRect(getLocalBounds());
	}
}

void PatchSheetLine::resized()
{
	int width = getWidth();
	int w = width / 10;
	//w = jmin(45, w);
	int h = 20;

	//isSelected.setBounds(0, 20, 0.5 * w, h);

	if (idControler!= nullptr) idControler->setBounds(0 * w, 0, w, h);
	if (nameControler!= nullptr) nameControler->setBounds(1 * w, 0, 2 * w, h);
	if (typeControler!= nullptr) typeControler->setBounds(3 * w, 0, 2 * w, h);
	if (universeControler!= nullptr) universeControler->setBounds(5 * w, 0, 2 * w, h);
	if (adressControler != nullptr) adressControler->setBounds(7 * w, 0, 2 * w - 20, h);
	if (enabledControler != nullptr) enabledControler->setBounds(9 * w - 19, 1, 18, h-2);

	actionsBtn.setBounds(width - (1 * w), 0, 1*w, h);

	finalHeight = 20;
}

void PatchSheetLine::updateContent()
{
	//cueIdLabel.setText(String(targetCue->id->floatValue()), juce::dontSendNotification);
	//cueNameLabel.setText(targetCue->niceName, juce::dontSendNotification);

	// float htpInDelay = int(100 * targetCue->htpInDelay->floatValue())/100.;
	// float htpOutDelay = int(100 * targetCue->htpOutDelay->floatValue()) / 100.;
	// float ltpDelay = int(100 * targetCue->ltpDelay->floatValue()) / 100.;
	// float htpInFade = int(100 * targetCue->htpInFade->floatValue()) / 100.;
	// float htpOutFade = int(100 * targetCue->htpOutFade->floatValue()) / 100.;
	// float ltpFade = int(100 * targetCue->ltpFade->floatValue()) / 100.;

	// htpUpDelayLabel.setText(htpInDelay < 0 ? "-" : String(htpInDelay), juce::dontSendNotification);
	// htpDownDelayLabel.setText(htpOutDelay < 0 ? "-" : String(htpOutDelay), juce::dontSendNotification);
	// ltpDelayLabel.setText(ltpDelay < 0 ? "-" : String(ltpDelay), juce::dontSendNotification);
	// htpUpFadeLabel.setText(htpInFade < 0 ? "-" : String(htpInFade), juce::dontSendNotification);
	// htpDownFadeLabel.setText(htpOutFade < 0 ? "-" : String(htpOutFade), juce::dontSendNotification);
	// ltpFadeLabel.setText(ltpFade < 0 ? "-" : String(ltpFade), juce::dontSendNotification);

}

void PatchSheetLine::buttonClicked(Button* b)
{
	ModifierKeys k = ModifierKeys::getCurrentModifiersRealtime();
	BKEngine::selectionMode m = BKEngine::SET;
	if (k.isCtrlDown()) m = BKEngine::ADD;
	if (k.isShiftDown()) m = BKEngine::ADDMULTIPLE;
	//dynamic_cast<BKEngine*>(BKEngine::mainEngine)->selectCue(targetCue, m);
}

void PatchSheetLine::changeListenerCallback(ChangeBroadcaster* source)
{
	Cue* c = dynamic_cast<Cue*>(source);
	if (c != nullptr) {
		updateContent();
	}
}

void PatchSheetLine::labelTextChanged(Label* l)
{
	//if (targetCue == nullptr) return;
	//if (l == &cueIdLabel) { targetCue->id->setValue(l->getText().getFloatValue()); }
	//if (l == &cueNameLabel) { targetCue->setNiceName(l->getText()); }
	//if (l == &htpUpDelayLabel) 
	//{ 
	//	float val = l->getText().trim() == "" ? -1 : l->getText().getFloatValue();
	//	targetCue->htpInDelay->setValue(val);
	//}
	//if (l == &htpDownDelayLabel)
	//{
	//	float val = l->getText().trim() == "" ? -1 : l->getText().getFloatValue();
	//	targetCue->htpOutDelay->setValue(val);
	//}
	//if (l == &ltpDelayLabel)
	//{
	//	float val = l->getText().trim() == "" ? -1 : l->getText().getFloatValue();
	//	targetCue->ltpDelay->setValue(val);
	//}
	//if (l == &htpUpFadeLabel)
	//{
	//	float val = l->getText().trim() == "" ? -1 : l->getText().getFloatValue();
	//	targetCue->htpInFade->setValue(val);
	//}
	//if (l == &htpDownFadeLabel)
	//{
	//	float val = l->getText().trim() == "" ? -1 : l->getText().getFloatValue();
	//	targetCue->htpOutFade->setValue(val);
	//}
	//if (l == &ltpFadeLabel)
	//{
	//	float val = l->getText().trim() == "" ? -1 : l->getText().getFloatValue();
	//	targetCue->ltpFade->setValue(val); 
	//}
}

void PatchSheetLine::showLinePopup()
{
	PopupMenu p;
	p.addItem("Inspect fixture", [this]() {targetFixture->selectThis(); });
	p.addItem("Inspect patch", [this]() {targetFixturePatch->selectThis(); });
	/*
	p.addItem("Load as next cue", [this, targetCuelist]() {targetCuelist->nextCue->setValueFromTarget(targetCue);});
	p.addItem("Load and go", [this, targetCuelist]() {targetCuelist->nextCue->setValueFromTarget(targetCue); targetCuelist->go();});
	p.addItem("Load and go instant", [this, targetCuelist]() {targetCuelist->nextCue->setValueFromTarget(targetCue); targetCuelist->go(0,0); });
	p.addSeparator();
	p.addItem("Load content",[this, targetCuelist](){targetCue->loadContent(UserInputManager::getInstance()->getProgrammer(true));});
	p.addSeparator();
	p.addItem("Insert before", [this, targetCuelist]() {targetCuelist->insertProgCueBefore(targetCue); });
	p.addItem("Insert after", [this, targetCuelist]() {targetCuelist->insertProgCueAfter(targetCue); });
	p.addSeparator();
	p.addItem("Replace", [this, targetCuelist]() {targetCue->replaceContent(UserInputManager::getInstance()->getProgrammer(true)); });
	p.addItem("Merge", [this, targetCuelist]() {targetCue->mergeContent(UserInputManager::getInstance()->getProgrammer(true)); });
	*/

	p.showMenuAsync(PopupMenu::Options(), [this](int result){});

}

void PatchSheetLine::mouseEnter(const MouseEvent& event)
{
	mouseOver = true;
	repaint();
}

void PatchSheetLine::mouseExit(const MouseEvent& event)
{
	mouseOver = false;
	repaint();
}

