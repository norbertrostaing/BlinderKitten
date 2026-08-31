/*
  ==============================================================================

    DMXChannelView.h
    Created: 8 Oct 2020 6:59:51pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "Definitions/Fixture/Fixture.h"
#include "Definitions/Fixture/FixturePatch.h"

class PatchSheet;

class PatchSheetLine :
    public Component,
    public ToggleButton::Listener,
    public ChangeListener,
    public Label::Listener
{
public:
    PatchSheetLine(Fixture* f, FixturePatch* fp, PatchSheet* parent);
    ~PatchSheetLine();
    
    PatchSheet* parentPatchSheet;
    Fixture* targetFixture = nullptr;
    FixturePatch* targetFixturePatch = nullptr;

    bool isRunning = false;
    bool mouseOver = false;

    int finalHeight = 30;

    IntParameterLabelUI* idControler = nullptr;
    StringParameterUI* nameControler = nullptr;
    TargetParameterUI* typeControler = nullptr;
    TargetParameterUI* universeControler = nullptr;
    IntParameterLabelUI* adressControler = nullptr;
    BoolButtonToggleUI* enabledControler = nullptr;

    ToggleButton isSelected;
    TextButton actionsBtn;

    void paint(Graphics& g) override;
    void resized() override;
    void updateContent();
    void buttonClicked(Button* b) override;

    void changeListenerCallback(ChangeBroadcaster* source) override;
    void labelTextChanged(Label* l) override;

    void showLinePopup();
    void mouseEnter(const MouseEvent& event) override;
    void mouseExit(const MouseEvent& event) override;
};
