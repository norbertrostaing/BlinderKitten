#pragma once

#include <JuceHeader.h>
#include "BKEngine.h"
#include "PatchSheetLine.h"

class Cuelist;

//==============================================================================
/*
*/
class PatchSheetUI : public ShapeShifterContent
{
public:
    PatchSheetUI(const String& contentName);
    ~PatchSheetUI();

    static PatchSheetUI* create(const String& name) { return new PatchSheetUI(name); }


};


class PatchSheet  : 
    public juce::Component,
    public ChangeListener
{
public:
    juce_DeclareSingleton(PatchSheet, true);
    PatchSheet();
    ~PatchSheet() override;

    TextButton csvExportBtn;
    TextButton inspectCuelistBtn;

    Label idLabel;
    Label nameLabel;
    Label typeLabel;
    Label universeLabel;
    Label adressLabel;

    ToggleButton isSelected;

    Viewport viewport;
    Component linesContainer;

    OwnedArray<PatchSheetLine> lines;

    void paint (juce::Graphics&) override;
    void resized() override;
    void updateStyle();
    void updateContent();
    void rebuildLines();

    void inspect(int delta);
    void inspectCuelist();

    void updateSelection();

    void changeListenerCallback(ChangeBroadcaster* source) override;

    void exportToCSV();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PatchSheet)
};
