/*
  ==============================================================================

    CuelistAction.h
    Created: 3 Feb 2022 10:15:35pm
    Author:  No

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include"../../Common/Action/Action.h"

class InterfaceAction :
    public Action
{
public:
    InterfaceAction(var params = var());
    ~InterfaceAction();

    enum ActionType { MIDI_SEND, OSC_SEND };
    ActionType actionType;

    TargetParameter* midiInterface = nullptr;
    TargetParameter* oscInterface = nullptr;

    enum MidiType { NOTE, CONTROLCHANGE, PITCHWHEEL};
    EnumParameter* midiType = nullptr;
    IntParameter* channel = nullptr;
    IntParameter* pitchOrNumber = nullptr;
    IntParameter* midiValue7b = nullptr;
    IntParameter* midiValue14b = nullptr;
    Point2DParameter* outputRange7b = nullptr;
    Point2DParameter* outputRange14b = nullptr;

    StringParameter* address = nullptr;
    FloatParameter* oscOutputValue = nullptr;
    Point2DParameter* outputRangeOSC = nullptr;

    BoolParameter* linkInput;

    void updateDisplay();
    void onContainerParameterChangedInternal(Parameter*);

    void triggerInternal() override;
    void setValueInternal(var value, String origin, int incrementIndex, bool isRelative) override;

    var getValue();

    static InterfaceAction* create(var params) { return new InterfaceAction(params); }

};