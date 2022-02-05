/*
  ==============================================================================

    CommonIncludes.cpp
    Created: 9 Sep 2021 7:03:49pm
    Author:  bkupe

  ==============================================================================
*/

#include "CommonIncludes.h"
// #include "MainIncludes.h"
// #include "Scene/SceneIncludes.h"
// #include "Object/ObjectIncludes.h"
// #include "Sequence/SequenceIncludes.h"
// #include "Effect/EffectIncludes.h"

#include "Action/Action.cpp"
#include "Action/ActionManager.cpp"

#include "ComponentSelector/ComponentSelector.cpp"
#include "ComponentSelector/ui/ComponentSelectorUI.cpp"

#include "Serial/lib/cobs/cobs.cpp"
#include "Serial/SerialDevice.cpp"
#include "Serial/SerialDeviceParameter.cpp"
#include "Serial/SerialManager.cpp"

#include "DMX/device/DMXDevice.cpp"
#include "DMX/device/DMXSerialDevice.cpp"
#include "DMX/device/DMXArtNetDevice.cpp"
#include "DMX/device/DMXEnttecProDevice.cpp"
#include "DMX/device/DMXOpenUSBDevice.cpp"
#include "DMX/DMXManager.cpp"

#include "Helpers/SceneHelpers.cpp"

#include "MIDI/MIDIDevice.cpp"
#include "MIDI/MIDIDeviceParameter.cpp"
#include "MIDI/MIDIManager.cpp"
#include "MIDI/MTCReceiver.cpp"
#include "MIDI/MTCSender.cpp"
#include "MIDI/ui/MIDIDeviceChooser.cpp"
#include "MIDI/ui/MIDIDeviceParameterUI.cpp"



#include "Zeroconf/ZeroconfManager.cpp"