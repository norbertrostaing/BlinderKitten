/*
  ==============================================================================

    ObjectManager.h
    Created: 26 Sep 2020 10:02:28am
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "CommandSelection.h"
//#include "../SubFixture/SubFixture.h"
class SubFixture;
class ChannelType;

class CommandSelectionManager :
    public BaseManager<CommandSelection>
{
public:
    CommandSelectionManager();
    ~CommandSelectionManager();
    CriticalSection computing;
    Array<SubFixture*> computedSelectedSubFixtures;
    Array<Fixture*> computedSelectedFixtures;
    HashMap<SubFixture*, float> subFixtureToPosition;
    void computeSelection();
    void computeSelection(Array<int> groupHistory);
    Array<ChannelType *> getControllableChannelsTypes();

    Trigger* getProgButton;
    Trigger* setProgButton;

    InspectableEditor* getEditorInternal(bool isRoot, Array<Inspectable*> inspectables);

    void triggerTriggered(Trigger* t) override;

};


class CommandSelectionManagerEditor :
    public GenericManagerEditor<CommandSelection>
{
public:
    CommandSelectionManagerEditor(CommandSelectionManager* item, bool isRoot);
    ~CommandSelectionManagerEditor();

    CommandSelectionManager* commandSel;
    std::unique_ptr<TriggerButtonUI> getProgBT;
    std::unique_ptr<TriggerButtonUI> setProgBT;

    void resizedInternalHeader(Rectangle<int>& r) override;
};

