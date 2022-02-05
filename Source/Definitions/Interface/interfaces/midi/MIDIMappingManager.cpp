/*
  ==============================================================================

    MIDIMappingManager.cpp
    Created: 12 Oct 2020 11:07:45am
    Author:  bkupe

  ==============================================================================
*/

MIDIMappingManager::MIDIMappingManager() :
    BaseManager("Mappings")
{
}

MIDIMappingManager::~MIDIMappingManager()
{
}

void MIDIMappingManager::handleNote(int channel, int pitch, int velocity)
{
    for (auto& i : items) i->handleNote(channel, pitch, velocity);
}

void MIDIMappingManager::handleCC(int channel, int number, int value)
{
    for (auto& i : items) i->handleCC(channel, number, value);
}
