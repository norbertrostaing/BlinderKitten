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

    HashMap<ChannelType*, String> lastOrigin;
    Label commandLine;

    OwnedArray<TextButton> filterBtns;
    Array<ChannelFamily*> availableFilters;
    Array<ChannelFamily*> selectedFilters;
    Array<ChannelType*> channels;

    TextButton HLBtn;
    TextButton blindBtn;
    TextButton btnMode;
    TextButton encoderRangeBtn;
    TextButton numbersOrNamesBtn;

    TextButton littleMoveLeftBtn;
    TextButton littleMoveRightBtn;
    TextButton bigMoveLeftBtn;
    TextButton bigMoveRightBtn;

    TextButton commandUpBtn;
    TextButton commandDownBtn;
    TextButton explodeCommandBtn;

    Label commandNumber;

    int nEncoders = 10;
    int encodersOffset = 0;
    int mode = 0;
    int numberOrNames = 0;

    void initEncoders();
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(Slider* slider) override;
    void buttonClicked (Button*);
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
    void mouseDoubleClick(const MouseEvent &e);
    void clearFilters();
    void toggleFilter(ChannelFamily* cf, bool forceSolo = false);

    void offsetEncoders(int n);
    void clear();


    int encoderRange = 0;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Encoders)
};
