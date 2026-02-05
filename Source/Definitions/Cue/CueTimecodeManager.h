/*
  ==============================================================================

    CueTimecodeManager.h
    Created: 11 Jan 2026 1:30:02pm
    Author:  Norb

  ==============================================================================
*/

#pragma once
#include "CueTimecode.h"
//#include "../SubFixture/SubFixture.h"
class SubFixture;
class ChannelType;

class CueTimecodeManager :
    public BaseManager<CueTimecode>
{
public:
    CueTimecodeManager(const juce::String& name);
    ~CueTimecodeManager();

    Trigger* addCurrentBtn;
    Trigger* replaceCurrentBtn;

    InspectableEditor* getEditorInternal(bool isRoot, Array<Inspectable*> inspectables) override;

    void triggerTriggered(Trigger* t) override;
    void addWithCurrent();
    void replaceWithCurrent();

    void addItemInternal(CueTimecode*, juce::var data) override;
    void addItemsInternal(juce::Array<CueTimecode*>, juce::var data) override;
    void removeItemInternal(CueTimecode*) override;
    void removeItemsInternal(juce::Array<CueTimecode*>) override;

};


class CueTimecodeManagerEditor :
    public GenericManagerEditor<CueTimecode>
{
public:
    CueTimecodeManagerEditor(CueTimecodeManager* item, bool isRoot);
    ~CueTimecodeManagerEditor();

    CueTimecodeManager* commandSel;
    std::unique_ptr<TriggerButtonUI> addBtn;
    std::unique_ptr<TriggerButtonUI> replaceBtn;

    void resizedInternalHeader(Rectangle<int>& r) override;

};

