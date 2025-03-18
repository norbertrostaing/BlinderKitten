/*
  ==============================================================================

    EncodersMult.h
    Created: 8 Oct 2020 6:59:51pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Definitions/Interface/InterfaceManager.h"
#include "Definitions/ChannelFamily/ChannelType/ChannelType.h"
#include "Definitions/Command/CommandManager.h"

class EncodersMult;
class Command;

class EncodersMultCmd :
    public Component,
    public ContainerAsyncListener,
    public Slider::Listener,
    public TextButton::Listener
{
public:
    EncodersMultCmd(EncodersMult * v, Command * c);
    ~EncodersMultCmd();
    
    EncodersMult* encodersMulView = nullptr;
    OwnedArray<FloatSliderUI> encoders;
    TextButton commandLineBtn;
    int nEncoders = 0;
    int calculatedHeight;
    int calculatedWidth;

    Command* targetCommand = nullptr;
    Array<ChannelType*> channels;
    TextButton explodeBtn;
    TextButton deleteBtn;

    void resetCommand(Command* c);
    void commandContentChanged();
    void commandValueChanged();
    void resized();
    void calcSize();

    void sliderValueChanged(Slider* slider) override;
   
    void newMessage(const ContainerAsyncEvent& e) override;
    void buttonClicked(Button* b);
};

