/*
  ==============================================================================

    DMXMappingManager.cpp
    Created: 12 Oct 2020 11:07:45am
    Author:  bkupe

  ==============================================================================
*/

#include "Interface/InterfaceIncludes.h"

DMXMappingManager::DMXMappingManager() :
    BaseManager("Mappings")
{
}

DMXMappingManager::~DMXMappingManager()
{
}

void DMXMappingManager::handleChannel(int channel, int value, String origin)
{
    for (auto& i : items) i->handleChannel(channel, value, origin);
}

