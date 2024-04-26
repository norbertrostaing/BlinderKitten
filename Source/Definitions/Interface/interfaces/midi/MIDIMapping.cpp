/*
  ==============================================================================

    MIDIMapping.cpp
    Created: 12 Oct 2020 11:07:59am
    Author:  bkupe

  ==============================================================================
*/

#include "Definitions/Interface/InterfaceIncludes.h"

MIDIMapping::MIDIMapping() :
    BaseItem("MIDI Mapping"),
    isValid(false),
    wasInRange(false)
{
    saveAndLoadRecursiveData = true;

    mode = addEnumParameter("Mode", "Set the mode of this mapping.");
    mode->addOption("Linear", LINEAR)->addOption("Encoder", ENCODER);
    
    midiType = addEnumParameter("Type", "Sets the type to check");
    midiType->addOption("Note", NOTE)->addOption("Control Change", CONTROLCHANGE)->addOption("Pitch wheel", PITCHWHEEL);

    channel = addIntParameter("Channel", "The channel to use for this mapping. 0 means all channels.", 1, 0, 16);
    pitchOrNumber = addIntParameter("Pitch Or Number", "The pitch (for notes) or number (for controlChange) to use for this mapping.", 0, 0, 127);
    
    inputRange7b = addPoint2DParameter("Input Range", "The range to get from input");
    inputRange7b->setBounds(0, 0, 127, 127);
    inputRange7b->setPoint(0, 127);
    inputRange14b = addPoint2DParameter("Input Range 14b", "The range to get from input");
    inputRange14b->setBounds(0, 0, 16383, 16383);
    inputRange14b->setPoint(0, 16383);

    //outputRange = addPoint2DParameter("Output Range", "The range to remap the value to.",false);
    //outputRange->setPoint(0, 1);

    upInputRange = addPoint2DParameter("Up range", "Values considered as up value for the encoder");
    upInputRange->setBounds(0, 0, 127, 127);
    upInputRange->setPoint(0, 10);
    downInputRange = addPoint2DParameter("Down range", "Values considered as down value for the encoder");
    downInputRange->setBounds(0, 0, 127, 127);
    downInputRange->setPoint(64, 75);
    encoderValueRange = addPoint2DParameter("Value range", "Delta value for the target element");
    encoderValueRange->setBounds(0, 0, 1, 1);
    encoderValueRange->setPoint(0, 0.1);

    learnMode = addBoolParameter("Learn", "When active, this will automatically set the channel and pitch/number to the next incoming message", false);
    learnMode->isSavable = false;
    learnMode->hideInEditor = true;

    learnRange = addBoolParameter("Learn range", "enable this and move your fader to adapt your input range", false);
    learnRange->isSavable = false;

    addChildControllableContainer(&actionManager);

    showInspectorOnSelect = false;
    updateDisplay();
}

MIDIMapping::~MIDIMapping()
{
}

void MIDIMapping::onContainerParameterChangedInternal(Parameter* p) {
    if (p == mode || p == midiType) {
        updateDisplay();
    }
    if (p == learnRange) {
        nextInputReinitRange = learnRange->boolValue();
    }
}

void MIDIMapping::updateDisplay()
{
    MappingMode m = mode->getValueDataAsEnum<MappingMode>();

    upInputRange->hideInEditor = m == LINEAR;
    downInputRange->hideInEditor = m == LINEAR;
    encoderValueRange->hideInEditor = m == LINEAR;

    pitchOrNumber->hideInEditor = midiType->getValueDataAsEnum<MidiType>() == PITCHWHEEL;
    inputRange7b->hideInEditor = midiType->getValueDataAsEnum<MidiType>() == PITCHWHEEL || m != LINEAR;
    inputRange14b->hideInEditor = midiType->getValueDataAsEnum<MidiType>() != PITCHWHEEL || m != LINEAR;

    learnRange->hideInEditor = m != LINEAR;

    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

void MIDIMapping::handleNote(int rcvChannel, int pitch, int velocity, String origin)
{
    if (learnMode->boolValue())
    {
        midiType->setValueWithData(NOTE);
        pitchOrNumber->setValue(pitch);
        channel->setValue(rcvChannel);
        learnMode->setValue(false);
    }

    if (!enabled->boolValue()) return;

    if (midiType->getValueDataAsEnum<MidiType>() != NOTE) return;
    if (channel->intValue() != rcvChannel && channel->intValue() != 0) return;
    if (pitchOrNumber->intValue() != pitch) return;

    if (learnRange->boolValue()) {
        if (nextInputReinitRange) {
            nextInputReinitRange = false;
            var v = var(); v.append(velocity); v.append(velocity);
            inputRange7b->setValue(v);
        }
        else {
            int min = jmin((int)inputRange7b->x, velocity);
            int max = jmax((int)inputRange7b->y, velocity);
            var v = var(); v.append(min); v.append(max);
            inputRange7b->setValue(v);
        }
    }
    else {
        float relValue = jmap<float>(jlimit<float>(inputRange7b->x, inputRange7b->y, velocity), inputRange7b->x, inputRange7b->y, 0, 1);
        processValue(relValue, origin);
    }
}

void MIDIMapping::handleCC(int rcvChannel, int number, int value, String origin)
{
    if (learnMode->boolValue())
    {
        midiType->setValueWithData(CONTROLCHANGE);
        channel->setValue(rcvChannel);
        pitchOrNumber->setValue(number);
        learnMode->setValue(false);
    }

    if (!enabled->boolValue()) return;
    if (midiType->getValueDataAsEnum<MidiType>() != CONTROLCHANGE) return;
    if (channel->intValue() != rcvChannel && channel->intValue() != 0) return;
    if (pitchOrNumber->intValue() != number) return;

    if (learnRange->boolValue()) {
        if (nextInputReinitRange) {
            nextInputReinitRange = false;
            var v = var(); v.append(value); v.append(value);
            inputRange7b->setValue(v);
        }
        else {
            int min = jmin((int)inputRange7b->x, value);
            int max = jmax((int)inputRange7b->y, value);
            var v = var(); v.append(min); v.append(max);
            inputRange7b->setValue(v);
        }
    }
    else {
        float relValue = jmap<float>(jlimit<float>(inputRange7b->x, inputRange7b->y, value), inputRange7b->x, inputRange7b->y, 0, 1);
        processValue(relValue, origin);
    }
}

void MIDIMapping::handlePitchWheel(int rcvChannel, int value, String origin)
{
    if (learnMode->boolValue())
    {
        midiType->setValueWithData(PITCHWHEEL);
        channel->setValue(rcvChannel);
        learnMode->setValue(false);
    }

    if (!enabled->boolValue()) return;
    if (midiType->getValueDataAsEnum<MidiType>() != PITCHWHEEL) return;
    if (channel->intValue() != rcvChannel && channel->intValue() != 0) return;

    if (learnRange->boolValue()) {
        if (nextInputReinitRange) {
            nextInputReinitRange = false;
            var v = var(); v.append(value); v.append(value);
            inputRange14b->setValue(v);
        }
        else {
            int min = jmin((int)inputRange14b->x, value);
            int max = jmax((int)inputRange14b->y, value);
            var v = var(); v.append(min); v.append(max);
            inputRange14b->setValue(v);
        }
    }
    else {
        float relValue = jmap<float>(jlimit<float>(inputRange14b->x, inputRange14b->y, value), inputRange14b->x, inputRange14b->y, 0, 1);
        processValue(relValue, origin);
    }
}

void MIDIMapping::processValue(float value, String origin) {
    bool isRelative = false;

    MappingMode m = mode->getValueDataAsEnum<MappingMode>();
    if (m == ENCODER) {

        float divider = midiType->getValueDataAsEnum<MidiType>() == PITCHWHEEL ? 16383 : 127;

        float upX = upInputRange->x / divider;
        float upY = upInputRange->y / divider;
        float downX = downInputRange->x / divider;
        float downY = downInputRange->y / divider;

        if (value >= upX && value <= upY) {
            isRelative = true;
            value = jmap<float>(value, upX, upY, encoderValueRange->x, encoderValueRange->y);
            handleValue(value, origin, true);
        }
        if (value >= downX && value <= downY) {
            isRelative = true;
            value = -jmap<float>(value, downX, downY, encoderValueRange->x, encoderValueRange->y);
            handleValue(value, origin, true);
        }
        if (value >= upY && value <= upX) {
            isRelative = true;
            value = jmap<float>(value, upY, upX, encoderValueRange->y, encoderValueRange->x);
            handleValue(value, origin, true);
        }
        if (value >= downY && value <= downX) {
            isRelative = true;
            value = -jmap<float>(value, downY, downX, encoderValueRange->y, encoderValueRange->x);
            handleValue(value, origin, true);
        }
    }
    else {
        handleValue(value, origin, false);
    }

}

void MIDIMapping::handleValue(float value, String origin, bool isRelative)
{
    actionManager.setValueAll(value, origin, isRelative);
    return;
}

InspectableEditor* MIDIMapping::getEditorInternal(bool isRoot, Array<Inspectable*> inspectables)
{
    return new MIDIMappingEditor(this, isRoot);
}
