/*
  ==============================================================================

    MIDIMappingManager.h
    Created: 12 Oct 2020 11:07:45am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class MIDIMappingManager :
    public BaseManager<MIDIMapping>
{
public:
    MIDIMappingManager();
    ~MIDIMappingManager();

    bool handleNote(int channel, int pitch, int velocity, String origin);
    bool handleCC(int channel, int number, int value, String origin);
    bool handlePitchWheel(int channel, int value, String origin);
};