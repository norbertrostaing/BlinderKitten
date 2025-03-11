/*
  ==============================================================================

    InputPanel.h
    Created: 19 Feb 2022 12:19:42am
    Author:  No

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
class BKEngine;
//==============================================================================
/*
*/

class InputPanelUI : public ShapeShifterContent
{
public:
    InputPanelUI(const String& contentName);
    ~InputPanelUI();

    static InputPanelUI* create(const String& name) { return new InputPanelUI(name); }


};


class InputPanel  : 
    public ControllableContainer,
    public juce::Component
{
public:
    juce_DeclareSingleton(InputPanel, true);
    InputPanel();
    ~InputPanel() override;

    BKEngine * engine = nullptr;

    Trigger* paramFixture; TriggerButtonUI* btnFixture;
    Trigger* paramGroup; TriggerButtonUI* btnGroup;
    Trigger* paramSub; TriggerButtonUI* btnSub;
    Trigger* paramCuelist; TriggerButtonUI* btnCuelist;
    Trigger* paramCue; TriggerButtonUI* btnCue;
    Trigger* paramPreset; TriggerButtonUI* btnPreset;
    Trigger* paramEffect; TriggerButtonUI* btnEffect;
    Trigger* paramCarousel; TriggerButtonUI* btnCarousel;
    Trigger* param1; TriggerButtonUI* btn1;
    Trigger* param2; TriggerButtonUI* btn2;
    Trigger* param3; TriggerButtonUI* btn3;
    Trigger* param4; TriggerButtonUI* btn4;
    Trigger* param5; TriggerButtonUI* btn5;
    Trigger* param6; TriggerButtonUI* btn6;
    Trigger* param7; TriggerButtonUI* btn7;
    Trigger* param8; TriggerButtonUI* btn8;
    Trigger* param9; TriggerButtonUI* btn9;
    Trigger* param0; TriggerButtonUI* btn0;
    Trigger* paramPlus; TriggerButtonUI* btnPlus;
    Trigger* paramMinus; TriggerButtonUI* btnMinus;
    Trigger* paramDot; TriggerButtonUI* btnDot;
    Trigger* paramThru; TriggerButtonUI* btnThru;
    Trigger* paramBackspace; TriggerButtonUI* btnBackspace;
    Trigger* paramEnter; TriggerButtonUI* btnEnter;
    Trigger* paramClear; TriggerButtonUI* btnClear;
    Trigger* paramRecord; TriggerButtonUI* btnRecord;
    Trigger* paramEdit; TriggerButtonUI* btnEdit;
    Trigger* paramCopy; TriggerButtonUI* btnCopy;
    Trigger* paramDelete; TriggerButtonUI* btnDelete;
    Trigger* paramAssistant; TriggerButtonUI* btnAssistant;

    BoolParameter* paramBlackOut; BoolButtonToggleUI* btnBlackout;
    BoolParameter* paramInLock; BoolButtonToggleUI* btnInLock;

    FloatParameter* paramGrandMaster; FloatSliderUI* sliderGrandMaster;

    void paint (juce::Graphics&) override;
    void resized() override;
    void triggerTriggered(Trigger* t) override;
    void parameterValueChanged(Parameter*p) override;


    void setGrandMaster(float value, String origin);
    void updateButtonsText();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InputPanel)
};
