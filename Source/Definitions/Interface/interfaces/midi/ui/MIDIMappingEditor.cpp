/*
  ==============================================================================

    MIDIMappingEditor.cpp
    Created: 12 Oct 2020 12:30:31pm
    Author:  bkupe

  ==============================================================================
*/

MIDIMappingEditor::MIDIMappingEditor(MIDIMapping* item, bool isRoot) :
    BaseItemEditor(item, isRoot),
    midiMapping(item)
{
    learnBT.reset(midiMapping->learnMode->createButtonToggle());
    addAndMakeVisible(learnBT.get());
}

MIDIMappingEditor::~MIDIMappingEditor()
{
}

void MIDIMappingEditor::resizedInternalHeaderItemInternal(Rectangle<int>& r)
{
    learnBT->setBounds(r.removeFromRight(80).reduced(2));
}
