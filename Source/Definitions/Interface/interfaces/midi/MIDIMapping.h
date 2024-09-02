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

    enum MappingMode{ LINEAR, ENCODER };
    EnumParameter * mode;
    
    enum MidiType { NOTE, CONTROLCHANGE, PITCHWHEEL };
    EnumParameter* midiType;
    IntParameter* channel;
    IntParameter* pitchOrNumber;

    Point2DParameter* upInputRange;
    Point2DParameter* downInputRange;
    Point2DParameter* encoderValueRange;

    Point2DParameter* inputRange7b;
    Point2DParameter* inputRange14b;
    //Point2DParameter* outputRange;

    BoolParameter* learnMode;
    BoolParameter* learnRange;

    bool nextInputReinitRange = false;

    bool isValid;
    bool wasInRange;

    ActionManager actionManager;

    bool handleNote(int channel, int pitch, int velocity, String origin, int incrementIndex);
    bool handleCC(int channel, int number, int value, String origin, int incrementIndex);
    bool handlePitchWheel(int channel, int value, String origin, int incrementIndex);
    void processValue(float value, String origin, int incrementIndex);
    void handleValue(float value, String origin, int incrementIndex, bool isRelative);

    void onContainerParameterChangedInternal(Parameter* p);

    void updateDisplay();

    InspectableEditor* getEditorInternal(bool isRoot, Array<Inspectable*> inspectables);
};