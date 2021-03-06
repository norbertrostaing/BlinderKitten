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
    Label commandLine;
    TextButton btnMode;

    TextButton encoderRangeBtn;
    TextButton littleMoveLeftBtn;
    TextButton littleMoveRightBtn;
    TextButton bigMoveLeftBtn;
    TextButton bigMoveRightBtn;
    Array<ChannelType*> channels;

    int nEncoders = 10;
    int encodersOffset = 0;
    int mode = 0;


    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(Slider* slider) override;
    void buttonClicked (Button*);
    void updateFilters();
    void updateEncoders();
    void updateModeButton();
    void updateRangeButton();
    void updateContentWithCommand(Command* c);

    int encoderRange = 0;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Encoders)
};
