/*
  ==============================================================================

    MIDIFeedback.cpp
    Created: 12 Oct 2020 11:07:59am
    Author:  bkupe

  ==============================================================================
*/

#include "Definitions/Interface/InterfaceIncludes.h"
#include "UI/VirtualButtons/VirtualButton.h"
#include "UI/VirtualFaders/VirtualFaderButton.h"

MIDIFeedback::MIDIFeedback() :
    BaseItem("MIDI Feedback"),
    isValid(false),
    wasInRange(false)
{

    feedbackSource = addEnumParameter("Source type", "Source data for this feedback");
    feedbackSource->addOption("Virtual fader", VFADER)
                    ->addOption("Virtual rotary", VROTARY)
                    ->addOption("Virtual fader above button", VABOVEBUTTON)
                    ->addOption("Virtual fader below button", VBELOWBUTTON)
                    ->addOption("Virtual button", VBUTTON)
                    ->addOption("Encoder", ENCODER)
        ;

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
    
    differentChannels = addBoolParameter("Different Channels", "use different channels for each value", false);
    onChannel = addIntParameter("On Channel", "Channel to send when the target is on", 1,1,16);
    onValue = addIntParameter("On Value", "Value to send when the target is on", 0,0,127);
    offChannel = addIntParameter("Off Channel", "Channel to send when the target is off", 1,1,16);
    offValue  = addIntParameter("Off Value", "Value to send when the target is off", 0,0,127);
    onLoadedChannel = addIntParameter("On Loaded Channel", "Channel to send when the cuelist is on and has a loaded cue", 1,1,16);
    onLoadedValue  = addIntParameter("On Loaded Value", "Value to send when the cuelist is on and has a loaded cue", 0,0,127);
    offLoadedChannel = addIntParameter("Off Loaded Channel", "Channel to send when the cuelist is off and has a loaded cue", 1,1,16);
    offLoadedValue  = addIntParameter("Off Loaded Value", "Value to send when the cuelist is off and has a loaded cue", 0,0,127);
    currentCueChannel = addIntParameter("Current Cue Channel", "Channel to send when the current cue of the cuelist is the one selected in the button", 1,1,16);
    currentCueValue  = addIntParameter("Current Cue Value", "Value to send when the current cue of the cuelist is the one selected in the button", 0,0,127);
    loadedCueChannel = addIntParameter("Loaded Cue Channel", "Channel to send when the loaded cue of the cuelist is the one selected in the button", 1,1,16);
    loadedCueValue  = addIntParameter("Loaded Cue Value", "Value to send when the loaded cue of the cuelist is the one selected in the button", 0,0,127);
    isGenericChannel = addIntParameter("Generic Action Channel", "Channel to send when the target is on", 1,1,16);
    isGenericValue  = addIntParameter("Generic Action Value", "Value to send when the target is on", 0,0,127);

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
    sourcePage->hideInEditor = source == ENCODER;
    sourceCol->hideInEditor = source == ENCODER;
    sourceRow->hideInEditor = source != VBUTTON;
    sourceNumber->hideInEditor = source != VROTARY && source != VABOVEBUTTON && source != VBELOWBUTTON && source != ENCODER;

    MidiType type = midiType->getValueDataAsEnum<MidiType>();
    pitchOrNumber->hideInEditor = type == PITCHWHEEL;

    bool isButton = source == VBUTTON || source == VABOVEBUTTON || source == VBELOWBUTTON;
    bool isComplex = isButton && differentChannels->boolValue();

    outputRange -> hideInEditor = isButton;
    onValue -> hideInEditor = !isButton;
    offValue -> hideInEditor = !isButton;
    onLoadedValue -> hideInEditor = !isButton;
    offLoadedValue->hideInEditor = !isButton;
    currentCueValue->hideInEditor = !isButton;
    loadedCueValue->hideInEditor = !isButton;
    isGenericValue -> hideInEditor = !isButton;

    onChannel->hideInEditor = !isComplex;
    offChannel->hideInEditor = !isComplex;
    onLoadedChannel->hideInEditor = !isComplex;
    offLoadedChannel->hideInEditor = !isComplex;
    currentCueChannel->hideInEditor = !isComplex;
    loadedCueChannel->hideInEditor = !isComplex;
    isGenericChannel->hideInEditor = !isComplex;

    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

void MIDIFeedback::onContainerParameterChangedInternal(Parameter* p)
{
    if (p == feedbackSource || p == midiType || p == differentChannels) {
        updateDisplay();
    }
}

void MIDIFeedback::processFeedback(String address, double value, String origin, bool logOutput)
{
    String localAddress = "";
    FeedbackSource source = feedbackSource->getValueDataAsEnum<FeedbackSource>();
    MIDIInterface* inter = dynamic_cast<MIDIInterface*>(parentContainer->parentContainer.get());

    bool valid = false;
    int sendValue = 0;
    int sendChannel = channel->intValue();

    bool sameDevice = inter->niceName == origin;
    sameDevice = sameDevice && origin != "";

    if (source == VFADER && !sameDevice) {
        localAddress = "/vfader/" + String(sourcePage->intValue()) + "/" + String(sourceCol->intValue());
        if (address == localAddress) {
            valid = true;
            sendValue = round(jmap(value, 0., 1., (double)outputRange->getValue()[0], (double)outputRange->getValue()[1]));
        }
    }
    else if (source == VROTARY && !sameDevice) {
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
            sendValue = value == VirtualButton::BTN_CURRENTCUE ? currentCueValue->intValue() : sendValue;
            sendValue = value == VirtualButton::BTN_LOADEDCUE ? loadedCueValue->intValue() : sendValue;
            sendValue = value == VirtualButton::BTN_GENERIC ? isGenericValue->intValue() : sendValue;
            if (differentChannels->boolValue()) {
                sendChannel = value == VirtualButton::BTN_ON ? onChannel->intValue() : sendChannel;
                sendChannel = value == VirtualButton::BTN_OFF ? offChannel->intValue() : sendChannel;
                sendChannel = value == VirtualButton::BTN_ON_LOADED ? onLoadedChannel->intValue() : sendChannel;
                sendChannel = value == VirtualButton::BTN_OFF_LOADED ? offLoadedChannel->intValue() : sendChannel;
                sendChannel = value == VirtualButton::BTN_CURRENTCUE ? currentCueChannel->intValue() : sendChannel;
                sendChannel = value == VirtualButton::BTN_LOADEDCUE ? loadedCueChannel->intValue() : sendChannel;
                sendChannel = value == VirtualButton::BTN_GENERIC ? isGenericChannel->intValue() : sendChannel;
            }
        }
    }
    else if (source == VABOVEBUTTON) {
        localAddress = "/vabovebutton/" + String(sourcePage->intValue()) + "/" + String(sourceCol->intValue()) + "/" + String(sourceNumber->intValue());
        if (address == localAddress) {
            valid = true;
            sendValue = 0;
            sendValue = value == VirtualFaderButton::BTN_ON ? onValue->intValue() : sendValue;
            sendValue = value == VirtualFaderButton::BTN_OFF ? offValue->intValue() : sendValue;
            sendValue = value == VirtualFaderButton::BTN_ON_LOADED ? onLoadedValue->intValue() : sendValue;
            sendValue = value == VirtualFaderButton::BTN_OFF_LOADED ? offLoadedValue->intValue() : sendValue;
            sendValue = value == VirtualFaderButton::BTN_CURRENTCUE ? currentCueValue->intValue() : sendValue;
            sendValue = value == VirtualFaderButton::BTN_LOADEDCUE ? loadedCueValue->intValue() : sendValue;
            sendValue = value == VirtualFaderButton::BTN_GENERIC ? isGenericValue->intValue() : sendValue;
            if (differentChannels->boolValue()) {
                sendChannel = value == VirtualButton::BTN_ON ? onChannel->intValue() : sendChannel;
                sendChannel = value == VirtualButton::BTN_OFF ? offChannel->intValue() : sendChannel;
                sendChannel = value == VirtualButton::BTN_ON_LOADED ? onLoadedChannel->intValue() : sendChannel;
                sendChannel = value == VirtualButton::BTN_OFF_LOADED ? offLoadedChannel->intValue() : sendChannel;
                sendChannel = value == VirtualButton::BTN_CURRENTCUE ? currentCueChannel->intValue() : sendChannel;
                sendChannel = value == VirtualButton::BTN_LOADEDCUE ? loadedCueChannel->intValue() : sendChannel;
                sendChannel = value == VirtualButton::BTN_GENERIC ? isGenericChannel->intValue() : sendChannel;
            }
        }
    }
    else if (source == VBELOWBUTTON) {
        localAddress = "/vbelowbutton/" + String(sourcePage->intValue()) + "/" + String(sourceCol->intValue()) + "/" + String(sourceNumber->intValue());
        if (address == localAddress) {
            valid = true;
            sendValue = 0;
            sendValue = value == VirtualFaderButton::BTN_ON ? onValue->intValue() : sendValue;
            sendValue = value == VirtualFaderButton::BTN_OFF ? offValue->intValue() : sendValue;
            sendValue = value == VirtualFaderButton::BTN_ON_LOADED ? onLoadedValue->intValue() : sendValue;
            sendValue = value == VirtualFaderButton::BTN_OFF_LOADED ? offLoadedValue->intValue() : sendValue;
            sendValue = value == VirtualFaderButton::BTN_CURRENTCUE ? currentCueValue->intValue() : sendValue;
            sendValue = value == VirtualFaderButton::BTN_LOADEDCUE ? loadedCueValue->intValue() : sendValue;
            sendValue = value == VirtualFaderButton::BTN_GENERIC ? isGenericValue->intValue() : sendValue;
            if (differentChannels->boolValue()) {
                sendChannel = value == VirtualButton::BTN_ON ? onChannel->intValue() : sendChannel;
                sendChannel = value == VirtualButton::BTN_OFF ? offChannel->intValue() : sendChannel;
                sendChannel = value == VirtualButton::BTN_ON_LOADED ? onLoadedChannel->intValue() : sendChannel;
                sendChannel = value == VirtualButton::BTN_OFF_LOADED ? offLoadedChannel->intValue() : sendChannel;
                sendChannel = value == VirtualButton::BTN_CURRENTCUE ? currentCueChannel->intValue() : sendChannel;
                sendChannel = value == VirtualButton::BTN_LOADEDCUE ? loadedCueChannel->intValue() : sendChannel;
                sendChannel = value == VirtualButton::BTN_GENERIC ? isGenericChannel->intValue() : sendChannel;
            }
        }
    }
    else if (source == ENCODER && !sameDevice) {
        localAddress = "/encoder/" + String(sourceNumber->intValue());
        if (address == localAddress) {
            valid = true;
            sendValue = round(jmap(value, 0., 1., (double)outputRange->getValue()[0], (double)outputRange->getValue()[1]));
        }
    }
    else {

    }

    if (valid && (sendValue != lastSentValue || sendChannel != lastSentChannel )) {
        auto dev = inter->deviceParam->outputDevice;
        if (dev == nullptr) {
            return;
        }
        lastSentValue = sendValue;
        lastSentChannel = sendChannel;
        if (midiType->getValueDataAsEnum<MidiType>() == NOTE) {
            if (logOutput) { LOG("send Note On chan " + String(sendChannel) + ", pitch " + String(pitchOrNumber->intValue()) + ", vel " + String(round(sendValue))); }
            dev->sendNoteOn(sendChannel, pitchOrNumber->intValue(), round(sendValue));
        }
        if (midiType->getValueDataAsEnum<MidiType>() == CONTROLCHANGE) {
            if (logOutput) { LOG("send CC chan " + String(sendChannel) + ", number " + String(pitchOrNumber->intValue()) + ", val " + String(round(sendValue))); }
            dev->sendControlChange(sendChannel, pitchOrNumber->intValue(), round(sendValue));
        }
        if (midiType->getValueDataAsEnum<MidiType>() == PITCHWHEEL) {
            if (logOutput) { LOG("send Pitch Wheel chan " + String(sendChannel) + ", val " + String(round(sendValue))); }
            dev->sendPitchWheel(sendChannel, round(sendValue));
        }
    }

}

