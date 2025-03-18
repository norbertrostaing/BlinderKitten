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
    public Inspectable::InspectableListener,
    public Slider::Listener
{
public:
    juce_DeclareSingleton(EncodersMult, true);
    EncodersMult();
    ~EncodersMult();

    Viewport viewport;
    Component cmdContainer;
    BaseManager<Command>* targetCommandManager = nullptr;
    Programmer* targetProgrammer = nullptr;
    void newMessage(const CommandManager::ManagerEvent& e) override;

    enum Align{EQUAL, ONLYLEFT, ONLYRIGHT, ONLYIN, ONLYOUT, BALANCED, BALANCEDSYM};

    Align currentAlign = EQUAL;

    TextButton btnSelectAll;
    TextButton btnSelectNone;
    TextButton btnAllEqual;
    TextButton btnMoveOnlyLeft;
    TextButton btnMoveOnlyRight;
    TextButton btnMoveOnlyIn;
    TextButton btnMoveOnlyOut;
    TextButton btnMoveBalanced;
    TextButton btnMoveBalancedSym;

    OwnedArray<EncodersMultCmd> commandItems;
    OwnedArray<Label> labels;
    OwnedArray<Slider> encoders;
    Slider sensitivity;
    Label sensitivityLabel;
    float currentSensitivity = 1;
    float setSensitivity(float v);
    Array<double> lastValues;
    Array<ChannelType*> channels;
    void resized() override;

    void targetChanged();
    void reconstructSubComponents();
    void reconstructNames();
    void mouseDown(const MouseEvent& event);
    void clear();
    static EncodersMult* create(const String& name) { return new EncodersMult(); }
    void sliderValueChanged(Slider* slider) override;
    void sliderDragEnded(Slider* slider) override;

    void selectAlign(Align a);
    void updateAlignButtons();
    void selectAll();
    void selectNone();
};
