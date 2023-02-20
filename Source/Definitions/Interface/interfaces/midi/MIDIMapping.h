/*
  ==============================================================================

    MIDIMapping.h
    Created: 12 Oct 2020 11:07:59am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class MIDIMapping :
    public BaseItem
{
public:
    MIDIMapping();
    ~MIDIMapping();

    enum MappingMode{ TRIGGER, ONOFF, TOGGLE, CONTINUOUS };
    EnumParameter * mode;
    
    enum MidiType { NOTE, CONTROLCHANGE };
    EnumParameter* midiType;
    IntParameter* channel;
    IntParameter* pitchOrNumber;

    Point2DParameter* inputRange;
    Point2DParameter* outputRange;

    BoolParameter* learnMode;

    bool isValid;
    bool wasInRange;

    ActionManager actionManager;

    void handleNote(int channel, int pitch, int velocity, String origin);
    void handleCC(int channel, int number, int value, String origin);
    void handleValue(int value, String origin);

    InspectableEditor* getEditorInternal(bool isRoot);
};