/*
  ==============================================================================

    DMXMapping.h
    Created: 12 Oct 2020 11:07:59am
    Author:  bkupe

  ==============================================================================
*/
#pragma once

class DMXMapping :
    public BaseItem
{
public:
    DMXMapping();
    ~DMXMapping();

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

    void handleChannel(int channel, int value, String origin);
    void handleValue(int value, String origin);

};