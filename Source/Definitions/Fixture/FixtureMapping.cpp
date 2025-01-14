/*
  ==============================================================================

    FixtureTypeChannel.cpp
    Created: 8 Nov 2021 7:28:28pm
    Author:  No

  ==============================================================================
*/

#include "FixtureMapping.h"
#include "../Interface/InterfaceIncludes.h"
// #include "../ChannelFamily/ChannelFamilyManager.h"
#include "Fixture.h"
#include "Definitions/FixtureType/FixtureType.h"
#include "UI/PatchSheet/PatchSheet.h"
#include "Definitions/ChannelFamily/ChannelFamilyManager.h"

FixtureMapping::FixtureMapping(var params) :
	BaseItem(params.getProperty("name", "Mapping")),
	objectType(params.getProperty("type", "FixtureMapping").toString()),
	objectData(params)
{

    subFixtureId = addIntParameter("Subfixture Id", "Id of the subfixture to listen", 0,0);
    channelType = addTargetParameter("Channel type", "Channel of the subfixture to listen", ChannelFamilyManager::getInstance());
    channelType->maxDefaultSearchLevel = 2;
    channelType->targetType = TargetParameter::CONTAINER;
    channelType->typesFilter.add("ChannelType");

    outputInterface = addTargetParameter("Interface", "output interface", InterfaceManager::getInstance());
    outputInterface->targetType = TargetParameter::CONTAINER;
    outputInterface->maxDefaultSearchLevel = 0;
    outputInterface->excludeTypesFilter.add("DMX");
    saveAndLoadRecursiveData = true;

    midiType = addEnumParameter("Midi Type", "Sets the type to send");
    midiType->addOption("Note", NOTE)->addOption("Control Change", CONTROLCHANGE)->addOption("Pitch wheel", PITCHWHEEL);

    midiChannel = addIntParameter("Midi Channel", "The channel to use for this feedback.", 1, 1, 16);
    midiPitchOrNumber = addIntParameter("Midi Pitch Or Number", "The pitch (for notes) or number (for controlChange) to use for this feedback.", 0, 0, 127);
    midiOutputRange7b = addPoint2DParameter("Midi Output Range", "The range to remap the value to.");
    midiOutputRange14b = addPoint2DParameter("Midi Output Range 14b", "The range to remap the value to.");

    oscOutputAddress = addStringParameter("Address", "Output address.", "");
    oscOutputRange = addPoint2DParameter("OSC Output Range", "The range to remap the value to.");

    midiOutputRange7b->setBounds(0, 0, 127, 127);
    midiOutputRange14b->setBounds(0, 0, 16383, 16383);
    midiOutputRange7b->setPoint(0, 127);
    midiOutputRange14b->setPoint(0, 16383);
    oscOutputRange->setPoint(0, 1);


	editorCanBeCollapsed = false;
};

FixtureMapping::~FixtureMapping()
{
}

void FixtureMapping::onContainerParameterChangedInternal(Parameter* p)
{
    if (outputInterface->targetContainer == nullptr) {
        outputInterfaceType = "";
    }
    else {
        Interface* i = dynamic_cast<Interface*>(outputInterface->targetContainer.get());
        outputInterfaceType = i->getTypeString();
    }
    updateDisplay();
}

void FixtureMapping::updateDisplay()
{
    bool isOSC = outputInterfaceType == "OSC";
    bool isMIDI = outputInterfaceType == "MIDI";

    bool isPitchWheel = isMIDI && midiType->getValueDataAsEnum<MidiType>() == PITCHWHEEL;

    midiType->hideInEditor = !isMIDI;
    midiChannel->hideInEditor = !isMIDI;
    midiPitchOrNumber->hideInEditor = !isMIDI || isPitchWheel;
    midiOutputRange7b->hideInEditor = !isMIDI || isPitchWheel;
    midiOutputRange14b->hideInEditor = !isMIDI || !isPitchWheel;

    oscOutputAddress->hideInEditor = !isOSC;
    oscOutputRange->hideInEditor = !isOSC;


    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

void FixtureMapping::process(int subId, ChannelType* type, double val)
{
    if (!enabled->boolValue()) return;
    if (channelType->targetContainer == nullptr) return;
    if (subId != subFixtureId->intValue() || dynamic_cast<ChannelType*>(channelType->targetContainer.get()) != type) return;

    if (outputInterfaceType == "OSC") {
        OSCInterface* out = dynamic_cast<OSCInterface*>(outputInterface->targetContainer.get());
        if (out == nullptr) return;
        OSCMessage msg(oscOutputAddress->stringValue());
        float outval = jmap(val, 0., 1., (double)oscOutputRange->getValue()[0], (double)oscOutputRange->getValue()[1]);
        msg.addFloat32(outval);
        out->sendOSC(msg);
    }
    else if (outputInterfaceType == "MIDI") {
        MIDIInterface* out = dynamic_cast<MIDIInterface*>(outputInterface->targetContainer.get());
        auto dev = out->deviceParam->outputDevice;
        if (dev == nullptr) {
            return;
        }
        if (midiType->getValueDataAsEnum<MidiType>() == CONTROLCHANGE) {
            int outVal = jmap(val, 0., 1., (double)midiOutputRange7b->getValue()[0], (double)midiOutputRange7b->getValue()[1]);
            dev->sendControlChange(midiChannel->intValue(), midiPitchOrNumber->intValue(), outVal);
        }
        else if (midiType->getValueDataAsEnum<MidiType>() == NOTE) {
            int outVal = jmap(val, 0., 1., (double)midiOutputRange7b->getValue()[0], (double)midiOutputRange7b->getValue()[1]);
            dev->sendNoteOn(midiChannel->intValue(), midiPitchOrNumber->intValue(), outVal);
        }
        else if (midiType->getValueDataAsEnum<MidiType>() == PITCHWHEEL) {
            int outVal = jmap(val, 0., 1., (double)midiOutputRange14b->getValue()[0], (double)midiOutputRange14b->getValue()[1]);
            dev->sendPitchWheel(midiChannel->intValue(), outVal);
        }
    }
}









FixtureMappingManager::FixtureMappingManager():
	BaseManager("Mappings")
{
}

FixtureMappingManager::FixtureMappingManager(const juce::String& name):
	BaseManager(name)
{
}

FixtureMappingManager::~FixtureMappingManager()
{
}

void FixtureMappingManager::askForMoveBefore(BaseItem*)
{
}

void FixtureMappingManager::askForMoveAfter(BaseItem*)
{
}

void FixtureMappingManager::addItemInternal(FixtureMapping* c, var data)
{
}

void FixtureMappingManager::askForDuplicateItem(BaseItem* item)
{
	BaseManager::askForDuplicateItem(item);
}

void FixtureMappingManager::setItemIndex(FixtureMapping* item, int newIndex, bool addToUndo)
{
}
