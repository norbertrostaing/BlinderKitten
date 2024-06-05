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
    bool used = false;
    for (auto& i : items) used = used || i->handleNote(channel, pitch, velocity, origin);
    return used;
}

bool MIDIMappingManager::handleCC(int channel, int number, int value, String origin)
{   
    bool used = false;
    for (auto& i : items) used = used || i->handleCC(channel, number, value, origin);
    return used;
}

bool MIDIMappingManager::handlePitchWheel(int channel, int value, String origin)
{   
    bool used = false;
    for (auto& i : items) used = used || i->handlePitchWheel(channel, value, origin);
    return used;
}
