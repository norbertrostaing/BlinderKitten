/*
  ==============================================================================

    MIDIFeedback.cpp
    Created: 12 Oct 2020 11:07:59am
    Author:  bkupe

  ==============================================================================
*/

#include "Definitions/Interface/InterfaceIncludes.h"
#include "UI/VirtualButtons/VirtualButton.h"

MIDIFeedback::MIDIFeedback() :
    BaseItem("MIDI Feedback"),
    isValid(false),
    wasInRange(false)
{

    feedbackSource = addEnumParameter("Source type", "Source data for this feedback");
    feedbackSource->addOption("Virtual Fader", VFADER)->addOption("Virtual rotary", VROTARY)->addOption("Virtual button", VBUTTON);

    sourceId = addIntParameter("Source ID", "ID of the source", 0);
    sourcePage = addIntParameter("Source Page", "Source page, 0 means current page", 0);
    sourceCol = addIntParameter("Source Column", "Source Column", 1);
    sourceRow = addIntParameter("Source Row", "Source Row", 1);
    sourceNumber = addIntParameter("Source Number", "Source number", 1);

    midiType = addEnumParameter("Midi Type", "Sets the type to send");
    midiType->addOption("Note", NOTE)->addOption("Control Change", CONTROLCHANGE)->addOption("Pitch wheel", PITCHWHEEL);

    channel = addIntParameter("Midi Channel", "The channel to use for this feedback.", 1, 1, 16);
    pitchOrNumber = addIntParameter("Midi Pitch Or Number", "The pitch (for notes) or number (for controlChange) to use for this feedback.", 0, 0, 127);
    outputRange = addPoint2DParameter("Midi Output Range", "The range to remap the value to.");
    outputRange->setPoint(0, 127);

    //inputRange = addPoint2DParameter("Input Range", "The range to get from input",false);
    //inputRange->setBounds(0, 0, 127, 127);
    //inputRange->setPoint(0, 127);
    
    onValue = addIntParameter("On Value", "Value to send when the target is on", 0);
    offValue = addIntParameter("Off Value", "Value to send when the target is off", 0);
    onLoadedValue = addIntParameter("On Loaded Value", "Value to send when the cuelist is on and has a loaded cue", 0);
    offLoadedValue = addIntParameter("Off Loaded Value", "Value to send when the cuelist is off and has a loaded cue", 0);
    isGenericValue = addIntParameter("Generic Action Value", "Value to send when the target is on", 0);

    saveAndLoadRecursiveData = true;

    learnMode = addBoolParameter("Learn", "When active, this will automatically set the channel and pitch/number to the next incoming message", false);
    learnMode->isSavable = false;
    learnMode->hideInEditor = true;

    showInspectorOnSelect = false;

    updateDisplay();
}

MIDIFeedback::~MIDIFeedback()
{
}

void MIDIFeedback::updateDisplay() {
    
    FeedbackSource source = feedbackSource->getValueDataAsEnum<FeedbackSource>();
    sourceId->hideInEditor = true ;
    sourcePage->hideInEditor = false;
    sourceCol->hideInEditor = false;
    sourceRow->hideInEditor = source != VBUTTON;
    sourceNumber->hideInEditor = source != VROTARY;

    MidiType type = midiType->getValueDataAsEnum<MidiType>();
    pitchOrNumber->hideInEditor = type == PITCHWHEEL;

    outputRange -> hideInEditor = source == VBUTTON;
    onValue -> hideInEditor = source != VBUTTON;
    offValue -> hideInEditor = source != VBUTTON;
    onLoadedValue -> hideInEditor = source != VBUTTON;
    offLoadedValue -> hideInEditor = source != VBUTTON;
    isGenericValue -> hideInEditor = source != VBUTTON;

    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

void MIDIFeedback::onContainerParameterChangedInternal(Parameter* p)
{
    if (p == feedbackSource || p == midiType) {
        updateDisplay();
    }
}

void MIDIFeedback::processFeedback(String address, double value)
{
    String localAddress = "";
    FeedbackSource source = feedbackSource->getValueDataAsEnum<FeedbackSource>();

    bool valid = false;
    int sendValue = 0;

    if (source == VFADER) {
        localAddress = "/vfader/" + String(sourcePage->intValue()) + "/" + String(sourceCol->intValue());
        if (address == localAddress) {
            valid = true;
            sendValue = round(jmap(value, 0., 1., (double)outputRange->getValue()[0], (double)outputRange->getValue()[1]));
        }
    }
    else if (source == VROTARY) {
        localAddress = "/vrotary/" + String(sourcePage->intValue()) + "/" + String(sourceCol->intValue()) + "/" + String(sourceNumber->intValue());
        if (address == localAddress) {
            valid = true;
            sendValue = round(jmap(value, 0., 1., (double)outputRange->getValue()[0], (double)outputRange->getValue()[1]));
        }
    }
    else if (source == VBUTTON) {
        localAddress = "/vbutton/" + String(sourcePage->intValue()) + "/" + String(sourceCol->intValue()) + "/" + String(sourceRow->intValue());
        if (address == localAddress) {
            valid = true;
            sendValue = 0;
            sendValue = value == VirtualButton::BTN_ON ? onValue->intValue() : sendValue;
            sendValue = value == VirtualButton::BTN_OFF ? offValue->intValue() : sendValue;
            sendValue = value == VirtualButton::BTN_ON_LOADED ? onLoadedValue->intValue() : sendValue;
            sendValue = value == VirtualButton::BTN_OFF_LOADED ? offLoadedValue->intValue() : sendValue;
            sendValue = value == VirtualButton::BTN_GENERIC ? isGenericValue->intValue() : sendValue;
        }
    }
    else {

    }

    if (valid) {
        MIDIInterface* inter = dynamic_cast<MIDIInterface*>(parentContainer->parentContainer.get());
        auto dev = inter->deviceParam->outputDevice;

        if (midiType->getValueDataAsEnum<MidiType>() == NOTE) {
            dev->sendNoteOn(channel->intValue(), pitchOrNumber->intValue(), round(sendValue));
        }
        if (midiType->getValueDataAsEnum<MidiType>() == CONTROLCHANGE) {
            dev->sendControlChange(channel->intValue(), pitchOrNumber->intValue(), round(sendValue));
        }
        if (midiType->getValueDataAsEnum<MidiType>() == PITCHWHEEL) {
            dev->sendPitchWheel(channel->intValue(), round(sendValue));
        }
    }

}

