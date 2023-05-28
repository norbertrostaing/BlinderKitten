/*
  ==============================================================================

    MIDIFeedback.cpp
    Created: 12 Oct 2020 11:07:59am
    Author:  bkupe

  ==============================================================================
*/

#include "Definitions/Interface/InterfaceIncludes.h"

MIDIFeedback::MIDIFeedback() :
    BaseItem("MIDI Feedback"),
    isValid(false),
    wasInRange(false)
{
    saveAndLoadRecursiveData = true;

    // mode = addEnumParameter("Mode", "Set the mode of this Feedback.");
    // mode->addOption("Continuous", CONTINUOUS)->addOption("Trigger", TRIGGER)->addOption("On / Off", ONOFF)->addOption("Toggle", TOGGLE);
    feedbackSource = addEnumParameter("Source type", "Source data for this feedback");
    feedbackSource->addOption("Virtual Fader", VFADER)->addOption("Virtual rotary", VROTARY);

    sourceId = addIntParameter("Source ID", "ID of the source", 0);
    sourcePage = addIntParameter("Source Page", "Source page", 0);
    sourceCol = addIntParameter("Source Column", "Source Column", 0);
    sourceNumber = addIntParameter("Source Number", "Source number", 0);

    midiType = addEnumParameter("Midi Type", "Sets the type to send");
    midiType->addOption("Note", NOTE)->addOption("Control Change", CONTROLCHANGE)->addOption("Pitch wheel", PITCHWHEEL);

    channel = addIntParameter("Midi Channel", "The channel to use for this feedback.", 1, 1, 16);
    pitchOrNumber = addIntParameter("Midi Pitch Or Number", "The pitch (for notes) or number (for controlChange) to use for this feedback.", 0, 0, 127);
    outputRange = addPoint2DParameter("Midi Output Range", "The range to remap the value to.");
    outputRange->setBounds(0, 0, 127, 127);
    outputRange->setPoint(0, 127);

    //inputRange = addPoint2DParameter("Input Range", "The range to get from input",false);
    //inputRange->setBounds(0, 0, 127, 127);
    //inputRange->setPoint(0, 127);
    

    learnMode = addBoolParameter("Learn", "When active, this will automatically set the channel and pitch/number to the next incoming message", false);
    learnMode->isSavable = false;
    learnMode->hideInEditor = true;

    showInspectorOnSelect = false;

    updateDisplay();
}

MIDIFeedback::~MIDIFeedback()
{
}

void MIDIFeedback::handleNote(int rcvChannel, int pitch, int velocity, String origin)
{
    if (learnMode->boolValue())
    {
        midiType->setValueWithData(NOTE);
        pitchOrNumber->setValue(pitch);
        learnMode->setValue(false);
    }

    if (!enabled->boolValue()) return;

    if (midiType->getValueDataAsEnum<MidiType>() != NOTE) return;
    if (channel->intValue() != rcvChannel) return;
    if (pitchOrNumber->intValue() != pitch) return;
    float relVal = jmap<float>(jlimit<float>(0, 127, velocity), 0, 127, 0, 1);
    handleValue(relVal, origin);
}

void MIDIFeedback::handleCC(int rcvChannel, int number, int value, String origin)
{
    if (learnMode->boolValue())
    {
        midiType->setValueWithData(CONTROLCHANGE);
        pitchOrNumber->setValue(number);
        learnMode->setValue(false);
    }

    if (!enabled->boolValue()) return;
    if (midiType->getValueDataAsEnum<MidiType>() != CONTROLCHANGE) return;
    if (channel->intValue() != rcvChannel) return;
    if (pitchOrNumber->intValue() != number) return;
    float relVal = jmap<float>(jlimit<float>(0, 127, value), 0, 127, 0, 1);
    handleValue(relVal, origin);
}

void MIDIFeedback::handlePitchWheel(int rcvChannel, int value, String origin)
{
    if (learnMode->boolValue())
    {
        midiType->setValueWithData(PITCHWHEEL);
        learnMode->setValue(false);
    }

    if (!enabled->boolValue()) return;
    if (midiType->getValueDataAsEnum<MidiType>() != PITCHWHEEL) return;
    if (channel->intValue() != rcvChannel) return;
    float relVal = jmap<float>(jlimit<float>(0, 16383, value), 0, 16383, 0, 1);
    handleValue(relVal, origin);
}

void MIDIFeedback::handleValue(float value, String origin)
{
    if (!enabled->boolValue()) return;
    return;
}

void MIDIFeedback::updateDisplay() {
    
    FeedbackSource source = feedbackSource->getValueDataAsEnum<FeedbackSource>();

    sourceId->hideInEditor = true ;
    sourcePage->hideInEditor = false;
    sourceCol->hideInEditor = false;
    sourceNumber->hideInEditor = source == VFADER;

    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

void MIDIFeedback::onContainerParameterChangedInternal(Parameter*)
{
    updateDisplay();
}

void MIDIFeedback::processFeedback(String address, double value)
{
    LOG("Process" + address + " "+String(value));
}

