/*
  ==============================================================================

    Encoders.h
    Created: 19 Feb 2022 12:19:42am
    Author:  No

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "BKEngine.h"

class ChannelType;
class Command;

//==============================================================================
/*
*/
class EncodersUI : public ShapeShifterContent
{
public:
    EncodersUI(const String& contentName);
    ~EncodersUI();

    static EncodersUI* create(const String& name) { return new EncodersUI(name); }

};


class Encoders : 
    public ControllableContainer,
    public juce::Component,
    juce::Slider::Listener,
    juce::TextButton::Listener
{
public:
    juce_DeclareSingleton(Encoders, true);
    Encoders();
    ~Encoders() override;

    BKEngine* engine = nullptr;

    OwnedArray<Slider> encoders;
    OwnedArray<Label> labels;
    Array<FloatParameter*> encodersParam;

    HashMap<ChannelType*, String> lastOrigin;

    StringParameter* paramCommandLine; StringParameterUI* uiCommandLine;
    StringParameter* paramCommandNumber; StringParameterUI* uiCommandNumber;

    OwnedArray<TextButton> filterBtns;
    Array<ChannelFamily*> availableFilters;
    Array<ChannelFamily*> selectedFilters;
    Array<ChannelType*> channels;

    BoolParameter* paramHighLight; BoolButtonToggleUI* btnHighLight;
    BoolParameter* paramBlind; BoolButtonToggleUI* btnBlind;
    Trigger* paramMode; TriggerUI* btnMode;
    Trigger* paramEncoderRange; TriggerUI* btnEncoderRange;
    Trigger* paramNumbersOrNames; TriggerUI* btnNumbersOrNames;

    Trigger* paramLittleMoveLeft; TriggerUI* btnLittleMoveLeft;
    Trigger* paramLittleMoveRight; TriggerUI* btnLittleMoveRight;
    Trigger* paramBigMoveLeft; TriggerUI* btnBigMoveLeft;
    Trigger* paramBigMoveRight; TriggerUI* btnBigMoveRight;

    Trigger* paramCommandDown; TriggerUI* btnCommandDown;
    Trigger* paramCommandUp; TriggerUI* btnCommandUp;
    Trigger* paramExplodeCommand; TriggerUI* btnExplodeCommand;


    int nEncoders = 10;
    int encodersOffset = 0;
    int mode = 0;
    int numberOrNames = 0;

    void initEncoders();
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(Slider* slider) override;
    void buttonClicked (Button*) override;
    void triggerTriggered(Trigger* t) override;
    void updateFilterBtns();
    void updateEncoders();
    void updateChannels();
    void updateModeButton();
    void updateNumbersOrNamesButton();
    void updateHLButton();
    void updateBlindButton();
    void updateRangeButton();
    void updateEncodersValues();
    void updateCommandLine();
    void mouseDoubleClick(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    void clearFilters();
    void toggleFilter(ChannelFamily* cf, bool forceSolo = false);

    void offsetEncoders(int n);
    void clear() override;

    void mouseDown(const MouseEvent& e) override;

    void parameterValueChanged(Parameter* p) override;

    bool disableNextExplode = false;
    bool transmitOrganicToEncoder = true;

    int encoderRange = 0;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Encoders)
};
