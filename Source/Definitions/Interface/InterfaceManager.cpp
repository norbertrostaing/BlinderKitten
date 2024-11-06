/*
  ==============================================================================

    InterfaceManager.cpp
    Created: 26 Sep 2020 1:50:28pm
    Author:  bkupe

  ==============================================================================
*/

#include "InterfaceManager.h"

#include "interfaces/dmx/DMXInterface.h"
#include "interfaces/osc/OSCInterface.h"
#include "interfaces/serial/SerialInterface.h"
#include "interfaces/bento/BentoInterface.h"
#include "interfaces/midi/MIDIInterface.h"

juce_ImplementSingleton(InterfaceManager)

InterfaceManager::InterfaceManager() :
    BaseManager("Interfaces")
{
    managerFactory = &factory;

    factory.defs.add(Factory<Interface>::Definition::createDef("", "DMX", &DMXInterface::create));
    factory.defs.add(Factory<Interface>::Definition::createDef("", "OSC", &OSCInterface::create));
    // factory.defs.add(Factory<Interface>::Definition::createDef("", "Serial", &SerialInterface::create));
    // factory.defs.add(Factory<Interface>::Definition::createDef("", "Bento", &BentoInterface::create));
    factory.defs.add(Factory<Interface>::Definition::createDef("", "MIDI", &MIDIInterface::create));
}

InterfaceManager::~InterfaceManager()
{
}

void InterfaceManager::feedback(String address, var value, String origin = "")
{
    Array<MIDIInterface*> midiInterfaces = getInterfacesOfType<MIDIInterface>();
    for (int i = 0; i < midiInterfaces.size(); i++) {
        midiInterfaces[i]->feedback(address, value, origin);
    }
    Array<OSCInterface*> oscInterfaces = getInterfacesOfType<OSCInterface>();
    for (int i = 0; i < oscInterfaces.size(); i++) {
        oscInterfaces[i]->feedback(address, value, origin);
    }
}
