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
#include "UI/VirtualButtons/VirtualButtonGrid.h"
#include "UI/VirtualFaders/VirtualFaderColGrid.h"

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
                    ->addOption("Grand Master", GRANDMASTER)
                    ->addOption("Black out", BLACKOUT)
        ;

    sourceId = addIntParameter("Source ID", "ID of the source", 0);
    sourcePage = addIntParameter("Source Page", "Source page, 0 means current page", 0);
    onlyIfCurrentPage = addBoolParameter("Only if current page", "if checked, the feedback will trigger only if the selected page if the current page", false);
    sourceCol = addIntParameter("Source Column", "Source Column", 1);
    sourceRow = addIntParameter("Source Row", "Source Row", 1);
    sourceNumber = addIntParameter("Source Number", "Source number", 1);

    midiType = addEnumParameter("Midi Type", "Sets the type to send");
    midiType->addOption("Note", NOTE)->addOption("Control Change", CONTROLCHANGE)->addOption("Pitch wheel", PITCHWHEEL)->addOption("Text", TEXT);

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

    textMode = addEnumParameter("Text mode", "");
    textMode->addOption("MCU Encoder", MCU_ENC)->addOption("MCU Fader", MCU_FADER)->addOption("MCU Encoder then Fader", MCU_ENCANDFADER);
    mackieColumn = addIntParameter("MCU column", "", 1, 1, 8);

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
    sourcePage->hideInEditor = source == ENCODER || source == GRANDMASTER || source == BLACKOUT;
    onlyIfCurrentPage->hideInEditor = source == ENCODER || source == GRANDMASTER || source == BLACKOUT || sourcePage->intValue()==0;
    sourceCol->hideInEditor = source == ENCODER || source == GRANDMASTER || source == BLACKOUT;
    sourceRow->hideInEditor = source != VBUTTON || source == GRANDMASTER || source == BLACKOUT;
    sourceNumber->hideInEditor = source != VROTARY && source != VABOVEBUTTON && source != VBELOWBUTTON && source != ENCODER;

    MidiType type = midiType->getValueDataAsEnum<MidiType>();
    pitchOrNumber->hideInEditor = type == PITCHWHEEL;

    bool isButton = source == VBUTTON || source == VABOVEBUTTON || source == VBELOWBUTTON;
    bool isComplex = isButton && differentChannels->boolValue();
    bool isText = type == TEXT;
    bool isBlackout = source == BLACKOUT;

    channel->hideInEditor = isText;
    pitchOrNumber->hideInEditor = isText;
    differentChannels->hideInEditor = !isButton;
    channel->hideInEditor = isButton && differentChannels->boolValue();

    outputRange -> hideInEditor = isButton || isText;
    onValue -> hideInEditor = !isButton && !isBlackout;
    offValue -> hideInEditor = !isButton && !isBlackout;
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

    textMode->hideInEditor = !isText;
    mackieColumn->hideInEditor = !isText;

    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

void MIDIFeedback::onContainerParameterChangedInternal(Parameter* p)
{
    if (p == feedbackSource || p == midiType || p == differentChannels || p == sourcePage) {
        updateDisplay();
    }
}

void MIDIFeedback::processFeedback(String address, var varValue, String origin, bool logOutput)
{
    if (!enabled->boolValue()) return;
    String localAddress = "";
    FeedbackSource source = feedbackSource->getValueDataAsEnum<FeedbackSource>();
    if (inter == nullptr) {
        inter = dynamic_cast<MIDIInterface*>(parentContainer->parentContainer.get());
    }

    bool valid = false;
    int sendValue = 0;
    int sendChannel = channel->intValue();

    bool sameDevice = inter->niceName == origin;
    sameDevice = sameDevice && origin != "";
    sameDevice = false;

    bool isText = varValue.isString();
    if (isText && midiType->getValueDataAsEnum<MidiType>() != TEXT) { return; }
    if (!isText && midiType->getValueDataAsEnum<MidiType>() == TEXT) { return; }

    int faderPage = VirtualFaderColGrid::getInstance()->page;
    int buttonPage = VirtualButtonGrid::getInstance()->page;
    int localPage = sourcePage->intValue();

    bool validPageButton = true;
    bool validPageFader = true;

    address = address.replace("/vfader/0/", "/vfader/" + String(faderPage) + "/");
    address = address.replace("/vrotary/0/", "/vrotary/" + String(faderPage) + "/");
    address = address.replace("/vbelowbutton/0/", "/vbelowbutton/" + String(faderPage) + "/");
    address = address.replace("/vabovebutton/0/", "/vabovebutton/" + String(faderPage) + "/");
    address = address.replace("/vbutton/0/", "/vbutton/" + String(buttonPage) + "/");

    if (onlyIfCurrentPage->boolValue()) {
        validPageButton = buttonPage == sourcePage->intValue() || sourcePage->intValue() == 0;
        validPageFader = faderPage == sourcePage->intValue() || sourcePage->intValue() == 0;
    }

    double floatValue = varValue;
    if (source == VFADER && !sameDevice && validPageFader) {
        localPage = localPage == 0 ? faderPage : localPage;
        localAddress = "/vfader/" + String(localPage) + "/" + String(sourceCol->intValue());
        if (address == localAddress) {
            if (isText) {
                sendText(varValue);
            }
            else {
                valid = true;
                sendValue = round(jmap(floatValue, 0., 1., (double)outputRange->getValue()[0], (double)outputRange->getValue()[1]));
            }
        }
    }
    else if (source == VROTARY && !sameDevice && validPageFader) {
        localPage = localPage == 0 ? faderPage : localPage;
        localAddress = "/vrotary/" + String(localPage) + "/" + String(sourceCol->intValue()) + "/" + String(sourceNumber->intValue());
        if (address == localAddress) {
            valid = true;
            sendValue = round(jmap(floatValue, 0., 1., (double)outputRange->getValue()[0], (double)outputRange->getValue()[1]));
        }
    }
    else if (source == VBUTTON && validPageButton) {
        localPage = localPage == 0 ? buttonPage : localPage;
        localAddress = "/vbutton/" + String(localPage) + "/" + String(sourceCol->intValue()) + "/" + String(sourceRow->intValue());
        if (address == localAddress) {
            valid = true;
            sendValue = 0;
            sendValue = floatValue == VirtualButton::BTN_ON ? onValue->intValue() : sendValue;
            sendValue = floatValue == VirtualButton::BTN_OFF ? offValue->intValue() : sendValue;
            sendValue = floatValue == VirtualButton::BTN_ON_LOADED ? onLoadedValue->intValue() : sendValue;
            sendValue = floatValue == VirtualButton::BTN_OFF_LOADED ? offLoadedValue->intValue() : sendValue;
            sendValue = floatValue == VirtualButton::BTN_CURRENTCUE ? currentCueValue->intValue() : sendValue;
            sendValue = floatValue == VirtualButton::BTN_LOADEDCUE ? loadedCueValue->intValue() : sendValue;
            sendValue = floatValue == VirtualButton::BTN_GENERIC ? isGenericValue->intValue() : sendValue;
            if (differentChannels->boolValue()) {
                sendChannel = floatValue == VirtualButton::BTN_ON ? onChannel->intValue() : sendChannel;
                sendChannel = floatValue == VirtualButton::BTN_OFF ? offChannel->intValue() : sendChannel;
                sendChannel = floatValue == VirtualButton::BTN_ON_LOADED ? onLoadedChannel->intValue() : sendChannel;
                sendChannel = floatValue == VirtualButton::BTN_OFF_LOADED ? offLoadedChannel->intValue() : sendChannel;
                sendChannel = floatValue == VirtualButton::BTN_CURRENTCUE ? currentCueChannel->intValue() : sendChannel;
                sendChannel = floatValue == VirtualButton::BTN_LOADEDCUE ? loadedCueChannel->intValue() : sendChannel;
                sendChannel = floatValue == VirtualButton::BTN_GENERIC ? isGenericChannel->intValue() : sendChannel;
            }
        }
    }
    else if (source == VABOVEBUTTON && validPageFader) {
        localPage = localPage == 0 ? faderPage : localPage;
        localAddress = "/vabovebutton/" + String(localPage) + "/" + String(sourceCol->intValue()) + "/" + String(sourceNumber->intValue());
        if (address == localAddress) {
            valid = true;
            sendValue = 0;
            sendValue = floatValue == VirtualFaderButton::BTN_ON ? onValue->intValue() : sendValue;
            sendValue = floatValue == VirtualFaderButton::BTN_OFF ? offValue->intValue() : sendValue;
            sendValue = floatValue == VirtualFaderButton::BTN_ON_LOADED ? onLoadedValue->intValue() : sendValue;
            sendValue = floatValue == VirtualFaderButton::BTN_OFF_LOADED ? offLoadedValue->intValue() : sendValue;
            sendValue = floatValue == VirtualFaderButton::BTN_CURRENTCUE ? currentCueValue->intValue() : sendValue;
            sendValue = floatValue == VirtualFaderButton::BTN_LOADEDCUE ? loadedCueValue->intValue() : sendValue;
            sendValue = floatValue == VirtualFaderButton::BTN_GENERIC ? isGenericValue->intValue() : sendValue;
            if (differentChannels->boolValue()) {
                sendChannel = floatValue == VirtualButton::BTN_ON ? onChannel->intValue() : sendChannel;
                sendChannel = floatValue == VirtualButton::BTN_OFF ? offChannel->intValue() : sendChannel;
                sendChannel = floatValue == VirtualButton::BTN_ON_LOADED ? onLoadedChannel->intValue() : sendChannel;
                sendChannel = floatValue == VirtualButton::BTN_OFF_LOADED ? offLoadedChannel->intValue() : sendChannel;
                sendChannel = floatValue == VirtualButton::BTN_CURRENTCUE ? currentCueChannel->intValue() : sendChannel;
                sendChannel = floatValue == VirtualButton::BTN_LOADEDCUE ? loadedCueChannel->intValue() : sendChannel;
                sendChannel = floatValue == VirtualButton::BTN_GENERIC ? isGenericChannel->intValue() : sendChannel;
            }
        }
    }
    else if (source == VBELOWBUTTON && validPageFader) {
        localPage = localPage == 0 ? faderPage : localPage;
        localAddress = "/vbelowbutton/" + String(localPage) + "/" + String(sourceCol->intValue()) + "/" + String(sourceNumber->intValue());
        if (address == localAddress) {
            valid = true;
            sendValue = 0;
            sendValue = floatValue == VirtualFaderButton::BTN_ON ? onValue->intValue() : sendValue;
            sendValue = floatValue == VirtualFaderButton::BTN_OFF ? offValue->intValue() : sendValue;
            sendValue = floatValue == VirtualFaderButton::BTN_ON_LOADED ? onLoadedValue->intValue() : sendValue;
            sendValue = floatValue == VirtualFaderButton::BTN_OFF_LOADED ? offLoadedValue->intValue() : sendValue;
            sendValue = floatValue == VirtualFaderButton::BTN_CURRENTCUE ? currentCueValue->intValue() : sendValue;
            sendValue = floatValue == VirtualFaderButton::BTN_LOADEDCUE ? loadedCueValue->intValue() : sendValue;
            sendValue = floatValue == VirtualFaderButton::BTN_GENERIC ? isGenericValue->intValue() : sendValue;
            if (differentChannels->boolValue()) {
                sendChannel = floatValue == VirtualButton::BTN_ON ? onChannel->intValue() : sendChannel;
                sendChannel = floatValue == VirtualButton::BTN_OFF ? offChannel->intValue() : sendChannel;
                sendChannel = floatValue == VirtualButton::BTN_ON_LOADED ? onLoadedChannel->intValue() : sendChannel;
                sendChannel = floatValue == VirtualButton::BTN_OFF_LOADED ? offLoadedChannel->intValue() : sendChannel;
                sendChannel = floatValue == VirtualButton::BTN_CURRENTCUE ? currentCueChannel->intValue() : sendChannel;
                sendChannel = floatValue == VirtualButton::BTN_LOADEDCUE ? loadedCueChannel->intValue() : sendChannel;
                sendChannel = floatValue == VirtualButton::BTN_GENERIC ? isGenericChannel->intValue() : sendChannel;
            }
        }
    }
    else if (source == ENCODER && !sameDevice) {
        localAddress = "/encoder/" + String(sourceNumber->intValue());
        if (address == localAddress) {
            valid = true;
            sendValue = round(jmap(floatValue, 0., 1., (double)outputRange->getValue()[0], (double)outputRange->getValue()[1]));
        }
    }
    else if (source == GRANDMASTER && !sameDevice) {
        localAddress = "/grandmaster";
        if (address == localAddress) {
            valid = true;
            sendValue = round(jmap(floatValue, 0., 1., (double)outputRange->getValue()[0], (double)outputRange->getValue()[1]));
        }
    }
    else if (source == BLACKOUT && !sameDevice) {
        localAddress = "/blackout";
        if (address == localAddress) {
            valid = true;
            sendValue = floatValue == 0 ? offValue->intValue() : onValue->intValue();
        }
    }

    if (valid) {
        auto dev = inter->deviceParam->outputDevice;
        if (dev == nullptr) {
            return;
        }
        int sendPitch = pitchOrNumber->intValue();
        sendValue = round(sendValue);

        if (midiType->getValueDataAsEnum<MidiType>() == NOTE && (dev->sentNote[sendChannel][sendPitch] != sendValue || lastSentChannel != sendChannel || lastSentValue != sendValue)) {
            if (logOutput) { LOG("send Note On chan " + String(sendChannel) + ", pitch " + String(sendPitch) + ", vel " + String(sendValue)); }
            dev->sendNoteOn(sendChannel, sendPitch, sendValue);
        }
        if (midiType->getValueDataAsEnum<MidiType>() == CONTROLCHANGE && (dev->sentCC[sendChannel][sendPitch] != sendValue || lastSentChannel != sendChannel || lastSentValue != sendValue)) {
            if (logOutput) { LOG("send CC chan " + String(sendChannel) + ", number " + String(sendPitch) + ", val " + String(sendValue)); }
            dev->sendControlChange(sendChannel, sendPitch, sendValue);
        }
        if (midiType->getValueDataAsEnum<MidiType>() == PITCHWHEEL && (dev->sentPW[sendChannel] != sendValue || lastSentChannel != sendChannel || lastSentValue != sendValue)) {
            if (logOutput) { LOG("send Pitch Wheel chan " + String(sendChannel) + ", val " + String(sendValue)); }
            dev->sendPitchWheel(sendChannel, sendValue);
        }
        lastSentValue = sendValue;
        lastSentChannel = sendChannel;
    }

}

void MIDIFeedback::sendText(String text)
{
    TextMode m = textMode->getValueDataAsEnum<TextMode>();
    int col = mackieColumn->intValue();
    if (m == MCU_FADER) {
        sendMCUFaderText(col, text);
    }
    else if (m == MCU_ENC) {
        sendMCUEncoderText(col, text);
    }
    else if (m == MCU_ENCANDFADER) {
        String enc = "";
        String fad = "";
        enc = text;
        if (text.length() > 7) {
            fad = text.substring(7);
        }

        sendMCUEncoderText(col, enc);
        sendMCUFaderText(col, fad);
    }
}

void MIDIFeedback::sendMCUFaderText(int col, String text)
{
    if (inter == nullptr) {
        inter = dynamic_cast<MIDIInterface*>(parentContainer->parentContainer.get());
    }
    auto dev = inter->deviceParam->outputDevice;
    if (dev == nullptr) {
        return;
    }
    uint8 index = ((col - 1) * 7) + 56;
    uint8 data[]{ 0x00,0x00,0x66,0x14,0x12, index, ' ' , ' ' , ' ' , ' ' , ' ' , ' ' , ' ' };
    Array<uint8> sysexData;
    for (int i = 0; i < 13; i++) {
        sysexData.add(data[i]);
    }
    for (int i = 0; i < jmin(7, text.length()); i++) {
        sysexData.set(6 + i, text[i]);
    }
    dev->sendSysEx(sysexData);

}

void MIDIFeedback::sendMCUEncoderText(int col, String text)
{
    if (inter == nullptr) {
        inter = dynamic_cast<MIDIInterface*>(parentContainer->parentContainer.get());
    }
    auto dev = inter->deviceParam->outputDevice;
    if (dev == nullptr) {
        return;
    }
    uint8 index = ((col - 1) * 7);
    uint8 data[]{ 0x00,0x00,0x66,0x14,0x12, index, ' ' , ' ' , ' ' , ' ' , ' ' , ' ' , ' ' };
    Array<uint8> sysexData;
    for (int i = 0; i < 13; i++) {
        sysexData.add(data[i]);
    }
    for (int i = 0; i < jmin(7, text.length()); i++) {
        sysexData.set(6 + i, text[i]);
    }
    dev->sendSysEx(sysexData);
}

