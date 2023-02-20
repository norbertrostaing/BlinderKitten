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

void MIDIMappingManager::handleNote(int channel, int pitch, int velocity, String origin)
{
    for (auto& i : items) i->handleNote(channel, pitch, velocity, origin);
}

void MIDIMappingManager::handleCC(int channel, int number, int value, String origin)
{
    for (auto& i : items) i->handleCC(channel, number, value, origin);
}
