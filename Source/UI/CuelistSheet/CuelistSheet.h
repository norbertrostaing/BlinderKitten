#pragma once

#include <JuceHeader.h>
#include "BKEngine.h"
#include "CuelistSheetLine.h"

class Cuelist;

//==============================================================================
/*
*/
class CuelistSheetUI : public ShapeShifterContent
{
public:
    CuelistSheetUI(const String& contentName);
    ~CuelistSheetUI();

    static CuelistSheetUI* create(const String& name) { return new CuelistSheetUI(name); }


};


class CuelistSheet  : 
    public juce::Component,
    public ChangeListener
{
public:
    juce_DeclareSingleton(CuelistSheet, true);
    CuelistSheet();
    ~CuelistSheet() override;

    BKEngine* engine = nullptr;

    Cuelist* targetCuelist = nullptr;

    TextButton labelAndTimeBtn;
    TextButton inspectBtn;
    ControllableUI* mainIdSlider;

    Label cueIdLabel;
    Label cueNameLabel;

    Label releaseTrackingLabel;

    Label htpUpDelayLabel;
    Label htpDownDelayLabel;
    Label ltpDelayLabel;

    Label htpUpFadeLabel;
    Label htpDownFadeLabel;
    Label ltpFadeLabel;

    ToggleButton isSelected;


    Viewport viewport;
    Component linesContainer;

    OwnedArray<CuelistSheetLine> lines;

    void paint (juce::Graphics&) override;
    void resized() override;
    void updateStyle();
    void updateContent();
    void rebuildLines();

    void inspect(int delta);
    void inspectCuelist();

    void updateSelection();

    void changeListenerCallback(ChangeBroadcaster* source) override;

    void cueDeleted(Cue* c);
    void updateRunningCue();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CuelistSheet)
};
