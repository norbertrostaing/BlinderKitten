/*
  ==============================================================================

    CueTimecodeManager.cpp
    Created: 11 Jan 2026 1:30:02pm
    Author:  Norb

  ==============================================================================
*/

#include "CueTimecodeManager.h"
#include "Cue.h"
#include "Definitions/Cuelist/Cuelist.h"
#include "Definitions/Interface/InterfaceIncludes.h"


CueTimecodeManager::CueTimecodeManager(const juce::String& name) :
	BaseManager(name)
{
	itemDataType = "CueTimecode";
	selectItemWhenCreated = false;

	addCurrentBtn = addTrigger("Add current.", "");
	replaceCurrentBtn = addTrigger("Replace with current.", "");
	addCurrentBtn->hideInEditor = true;
	replaceCurrentBtn->hideInEditor = true;

}

CueTimecodeManager::~CueTimecodeManager()
{
}

InspectableEditor* CueTimecodeManager::getEditorInternal(bool isRoot, Array<Inspectable*> inspectables)
{
	return new CueTimecodeManagerEditor(this, isRoot);
}

void CueTimecodeManager::triggerTriggered(Trigger* t)
{
	if (t == addCurrentBtn) {
		addWithCurrent();
	}
	else if (t == replaceCurrentBtn) {
		replaceWithCurrent();
	}
}

void CueTimecodeManager::addWithCurrent()
{
	MessageManagerLock lock;
	Cue* c = dynamic_cast<Cue*>(parentContainer.get());
	if(c == nullptr) return;
	c->checkParentCuelist();
	if(c->parentCuelist == nullptr) return;
	if (c->parentCuelist->currentMidiTimecodeSyncInterface ==nullptr) return;
	int currentTC = c->parentCuelist->currentMidiTimecodeSyncInterface->lastFrameSent;
	int frame = currentTC%30;
	int second = (currentTC / 30) % 60;
	int minute = (currentTC / (30*60)) % 60;
	int hour = currentTC /(30*60*60);

	CueTimecode* ct = new CueTimecode();
	ct->frame->setValue(frame);
	ct->second->setValue(second);
	ct->minute->setValue(minute);
	ct->hour->setValue(hour);

	addItem(ct);

	LOG("add current");
}

void CueTimecodeManager::replaceWithCurrent()
{
	MessageManagerLock lock;
	clear();
	addWithCurrent();
}

void CueTimecodeManager::addItemInternal(CueTimecode*, juce::var data)
{
}

void CueTimecodeManager::addItemsInternal(juce::Array<CueTimecode*>, juce::var data)
{
}

void CueTimecodeManager::removeItemInternal(CueTimecode*)
{
}

void CueTimecodeManager::removeItemsInternal(juce::Array<CueTimecode*>)
{
}




CueTimecodeManagerEditor::CueTimecodeManagerEditor(CueTimecodeManager* item, bool isRoot) :
	GenericManagerEditor(item, isRoot),
	commandSel(item)
{
	addBtn.reset(commandSel->addCurrentBtn->createButtonUI());
	replaceBtn.reset(commandSel->replaceCurrentBtn->createButtonUI());
	addAndMakeVisible(addBtn.get());
	addAndMakeVisible(replaceBtn.get());
}

CueTimecodeManagerEditor::~CueTimecodeManagerEditor()
{
}

void CueTimecodeManagerEditor::resizedInternalHeader(Rectangle<int>& r)
{
	GenericManagerEditor::resizedInternalHeader(r);
	addBtn->setBounds(r.withTrimmedRight(80).removeFromRight(80).reduced(2));
	replaceBtn->setBounds(r.removeFromRight(80).reduced(2));
}
