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
#include "Definitions/Command/Command.h"
#include "Definitions/Command/CommandManager.h"

class EncodersMultCmd; 

class EncodersMult :
    public juce::Component,
    public CommandManager::AsyncListener,
    public Inspectable::InspectableListener
{
public:
    juce_DeclareSingleton(EncodersMult, true);
    EncodersMult();
    ~EncodersMult();

    Label test;
    Viewport viewport;
    Component cmdContainer;
    BaseManager<Command>* targetCommandManager = nullptr;
    void newMessage(const CommandManager::ManagerEvent& e) override;

    OwnedArray<EncodersMultCmd> commandItems;
    void resized() override;

    void targetChanged();
    void reconstructSubComponents();
    void reconstructNames();
    void mouseDown(const MouseEvent& event);
    void clear();
    static EncodersMult* create(const String& name) { return new EncodersMult(); }
};