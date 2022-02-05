/*
  ==============================================================================

    MIDIMappingEditor.h
    Created: 12 Oct 2020 12:30:31pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class MIDIMappingEditor :
    public BaseItemEditor
{
public:
    MIDIMappingEditor(MIDIMapping * item, bool isRoot);
    ~MIDIMappingEditor();

    MIDIMapping* midiMapping;
    std::unique_ptr<BoolButtonToggleUI> learnBT;

    void resizedInternalHeaderItemInternal(Rectangle<int>& r) override;
};