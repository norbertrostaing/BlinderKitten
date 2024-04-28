/*
  ==============================================================================

    DMXChannelView.h
    Created: 8 Oct 2020 6:59:51pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "Definitions/Cue/Cue.h"
class CuelistSheet;

class CuelistSheetLine :
    public Component,
    public ToggleButton::Listener,
    public ChangeListener,
    public Label::Listener
{
public:
    CuelistSheetLine(Cue* c, CuelistSheet* parent);
    ~CuelistSheetLine();
    
    CuelistSheet* parentCuelistSheet;
    Cue* targetCue;

    int finalHeight;

    bool isRunning = false;
    bool mouseOver = false;

    Label cueIdLabel;
    Label cueNameLabel;
    
    Label htpUpDelayLabel;
    Label htpDownDelayLabel;
    Label ltpDelayLabel;

    Label htpUpFadeLabel;
    Label htpDownFadeLabel;
    Label ltpFadeLabel;

    ToggleButton isSelected;
    TextButton actionsBtn;

    void paint(Graphics& g) override;
    void resized() override;
    void updateContent();
    void buttonClicked(Button* b) override;

    void changeListenerCallback(ChangeBroadcaster* source) override;
    void labelTextChanged(Label* l);

    void showCuePopup();
    void mouseEnter(const MouseEvent& event) override;
    void mouseExit(const MouseEvent& event) override;
};
