/*
  ==============================================================================

    MIDIMappingManager.cpp
    Created: 12 Oct 2020 11:07:45am
    Author:  bkupe

  ==============================================================================
*/

#include "Interface/InterfaceIncludes.h"

MIDIMappingManager::MIDIMappingManager() :
    BaseManager("Mappings")
{
}

MIDIMappingManager::~MIDIMappingManager()
{
}

bool MIDIMappingManager::handleNote(int channel, int pitch, int velocity, String origin)
{   
    String n = "n-"+String(channel)+"-"+String(pitch);
    int incrementIndex = 0;
    if (paramToIncrement.contains(n)) {incrementIndex = paramToIncrement.getReference(n); }
    incrementIndex++;
    paramToIncrement.set(n, incrementIndex);
    bool used = false;
    for (auto& i : items) used = i->handleNote(channel, pitch, velocity, origin, incrementIndex) || used;
    return used;
}

bool MIDIMappingManager::handleCC(int channel, int number, int value, String origin)
{   
    String n = "c-" + String(channel) + "-" + String(number);
    int incrementIndex = 0;
    if (paramToIncrement.contains(n)) { incrementIndex = paramToIncrement.getReference(n); }
    incrementIndex++;
    paramToIncrement.set(n, incrementIndex);
    bool used = false;
    for (auto& i : items) used = i->handleCC(channel, number, value, origin, incrementIndex) || used;
    return used;
}

bool MIDIMappingManager::handlePitchWheel(int channel, int value, String origin)
{   
    String n = "p-" + String(channel);
    int incrementIndex = 0;
    if (paramToIncrement.contains(n)) { incrementIndex = paramToIncrement.getReference(n); }
    incrementIndex++;
    paramToIncrement.set(n, incrementIndex);
    bool used = false;
    for (auto& i : items) used = i->handlePitchWheel(channel, value, origin, incrementIndex) || used;
    return used;
}
