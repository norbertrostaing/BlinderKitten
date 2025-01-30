/*
  ==============================================================================

    InterfaceAction.cpp
    Created: 3 Feb 2022 10:15:35pm
    Author:  No

  ==============================================================================
*/

#include "InterfaceAction.h"
#include "Definitions/Interface/InterfaceIncludes.h"

InterfaceAction::InterfaceAction(var params) :
    Action(params)
{
    actionType = (ActionType)(int)params.getProperty("actionType", MIDI_SEND);
    saveAndLoadRecursiveData = true;

    if (actionType == MIDI_SEND) {

        midiInterface = addTargetParameter("MIDI Interface", "Output interface, leave empty means all interfaces.", InterfaceManager::getInstance());
        midiInterface->targetType = TargetParameter::CONTAINER;
        midiInterface->customGetTargetContainerFunc = &InterfaceManager::showAndGetInterfaceOfType<MIDIInterface>;
        midiInterface->maxDefaultSearchLevel = 0;

        midiType = addEnumParameter("Midi Type", "Sets the type to send");
        midiType->addOption("Note", NOTE)->addOption("Control Change", CONTROLCHANGE)->addOption("Pitch wheel", PITCHWHEEL);

        channel = addIntParameter("Midi Channel", "The channel to use for this feedback.", 1, 1, 16);
        pitchOrNumber = addIntParameter("Midi Pitch Or Number", "The pitch (for notes) or number (for controlChange) to use for this feedback.", 0, 0, 127);
        linkInput = addBoolParameter("Link value with input", "", false);
        midiValue7b = addIntParameter("Midi Value 7b", "", 0, 0, 127);
        midiValue14b = addIntParameter("Midi Value 14b", "", 0, 0, 16383);
        outputRange7b = addPoint2DParameter("Midi Output Range", "The range to remap the value to.");
        outputRange14b = addPoint2DParameter("Midi Output Range 14b", "The range to remap the value to.");
        outputRange7b->setBounds(0, 0, 127, 127);
        outputRange7b->setPoint(0, 127);
        outputRange14b->setBounds(0, 0, 16383, 16383);
        outputRange14b->setPoint(0, 16383);
    }
    else if (actionType == OSC_SEND) {
        oscInterface = addTargetParameter("OSC Interface", "Output interface, leave empty means all interfaces.", InterfaceManager::getInstance());
        oscInterface->targetType = TargetParameter::CONTAINER;
        oscInterface->customGetTargetContainerFunc = &InterfaceManager::showAndGetInterfaceOfType<OSCInterface>;
        oscInterface->maxDefaultSearchLevel = 0;

        address = addStringParameter("Address", "OSC address you wanna send", "/example");
        oscType = addEnumParameter("Value type", "Sets the type to send");
        oscType->addOption("Trigger", TRIGGER)->addOption("Boolean", BOOLEAN)->addOption("Integer", INTEGER)->addOption("Float", FLOAT)->addOption("String", STRING);
        linkInput = addBoolParameter("Link value with input", "", false);
        oscOutputValueBool = addBoolParameter("Bool Output Value", "Value you wanna send", 0);
        oscOutputValueInt = addIntParameter("Int Output Value", "Value you wanna send", 0);
        oscOutputValueFloat = addFloatParameter("Float Output Value", "Value you wanna send", 0);
        oscOutputValueString = addStringParameter("String Output Value", "Value you wanna send", "");
        outputRangeOSC = addPoint2DParameter("OSC Output Range", "The range to remap the value to.");
    }
    updateDisplay();
}

InterfaceAction::~InterfaceAction()
{
}

void InterfaceAction::updateDisplay()
{
    if (actionType == MIDI_SEND) {
        midiValue7b->hideInEditor = linkInput->boolValue() || midiType->getValueDataAsEnum<MidiType>() == PITCHWHEEL;
        midiValue14b->hideInEditor = linkInput->boolValue() || midiType->getValueDataAsEnum<MidiType>() != PITCHWHEEL;

        outputRange7b->hideInEditor = !linkInput->boolValue() || midiType->getValueDataAsEnum<MidiType>() == PITCHWHEEL;
        outputRange14b->hideInEditor = !linkInput->boolValue() || midiType->getValueDataAsEnum<MidiType>() != PITCHWHEEL;

        pitchOrNumber->hideInEditor = midiType->getValueDataAsEnum<MidiType>() == PITCHWHEEL;
    }
    else if (actionType == OSC_SEND) {
        OscType t = oscType->getValueDataAsEnum<OscType>();
        
        linkInput->hideInEditor = t != BOOLEAN && t != INTEGER && t != FLOAT;

        oscOutputValueBool->hideInEditor = t != BOOLEAN || linkInput->boolValue();
        oscOutputValueInt->hideInEditor = t != INTEGER || linkInput->boolValue();
        oscOutputValueFloat->hideInEditor = t != FLOAT || linkInput->boolValue();
        oscOutputValueString->hideInEditor = t != STRING;

        outputRangeOSC->hideInEditor = !((t == INTEGER || t == FLOAT) && linkInput->boolValue());
    }
    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

void InterfaceAction::onContainerParameterChangedInternal(Parameter* p)
{
    if (p == linkInput || p == midiType || p == oscType) {
        updateDisplay();
    }
}

void InterfaceAction::triggerInternal()
{
}

void InterfaceAction::setValueInternal(var value, String origin, int incrementIndex, bool isRelative) {
    if (actionType == MIDI_SEND) {
        Array<MIDIInterface*> interfaces;
        MIDIInterface* inter = dynamic_cast<MIDIInterface*>(midiInterface->targetContainer.get());
        if (inter == nullptr) {
            interfaces = InterfaceManager::getInstance()->getInterfacesOfType<MIDIInterface>();
        }
        else {
            interfaces.add(inter);
        }

        int sendChannel = channel->intValue();
        int sendPitch = pitchOrNumber->intValue();
        int sendValue = 0;
        if (linkInput->boolValue()){
            if (midiType->getValueDataAsEnum<MidiType>() == PITCHWHEEL) {
                sendValue = jmap((float)value, 0.0f, 1.0f, outputRange14b->x, outputRange14b->y);
            }
            else {
                sendValue = jmap((float)value, 0.0f, 1.0f, outputRange7b->x, outputRange7b->y);
            }
        }
        else {
            sendValue = midiType->getValueDataAsEnum<MidiType>() == PITCHWHEEL ? midiValue14b->intValue() : midiValue7b->intValue();
        }

        for (MIDIInterface* i : interfaces) {
            auto dev = i->deviceParam->outputDevice;
            if (dev == nullptr) {
                continue;
            }

            if (midiType->getValueDataAsEnum<MidiType>() == NOTE) {
                dev->sendNoteOn(sendChannel, sendPitch, sendValue);
            }
            if (midiType->getValueDataAsEnum<MidiType>() == CONTROLCHANGE) {
                dev->sendControlChange(sendChannel, sendPitch, sendValue);
            }
            if (midiType->getValueDataAsEnum<MidiType>() == PITCHWHEEL) {
                dev->sendPitchWheel(sendChannel, sendValue);
            }
        }

    }
    else if (actionType == OSC_SEND) {
        Array<OSCInterface*> interfaces;
        OSCInterface* inter = dynamic_cast<OSCInterface*>(oscInterface->targetContainer.get());
        if (inter == nullptr) {
            interfaces = InterfaceManager::getInstance()->getInterfacesOfType<OSCInterface>();
        }
        else {
            interfaces.add(inter);
        }

        OSCMessage msg(address->stringValue());

        OscType t = oscType->getValueDataAsEnum<OscType>();
        bool link = linkInput->boolValue();
        if (!link && (float)value != 1) return;

        if (t == BOOLEAN) {
            bool sendValue = link ? (float)value == 1 : oscOutputValueBool->boolValue();
            msg.addArgument(OSCHelpers::varToArgument(sendValue, OSCHelpers::TF));
        }
        else if (t == INTEGER) {
            int sendValue = link ? round(jmap((float)value, 0.0f, 1.0f, outputRangeOSC->x, outputRangeOSC->y)) : oscOutputValueInt->intValue();
            msg.addArgument(OSCHelpers::varToArgument(sendValue, OSCHelpers::TF));
        }
        else if (t == FLOAT) {
            float sendValue = link ? jmap((float)value, 0.0f, 1.0f, outputRangeOSC->x, outputRangeOSC->y) : oscOutputValueFloat->floatValue();
            msg.addArgument(OSCHelpers::varToArgument(sendValue, OSCHelpers::TF));
        }
        else if(t == STRING) {
            msg.addArgument(OSCHelpers::varToArgument(oscOutputValueString->stringValue(), OSCHelpers::TF));
        }

        for (OSCInterface* i : interfaces) {
            i->sendOSC(msg);
        }

    }
}

var InterfaceAction::getValue()
{
    return var();
}
