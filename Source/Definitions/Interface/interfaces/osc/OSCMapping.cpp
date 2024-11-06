/*
  ==============================================================================

    OSCMapping.cpp
    Created: 12 Oct 2020 11:07:59am
    Author:  bkupe

  ==============================================================================
*/

#include "Definitions/Interface/InterfaceIncludes.h"
#include "OSCMapping.h"

OSCMapping::OSCMapping() :
    BaseItem("OSC Mapping"),
    isValid(false),
    wasInRange(false)
{
    saveAndLoadRecursiveData = true;

    inputAddress = addStringParameter("Address", "Input address", "");

    inputRange = addPoint2DParameter("Input Range", "The range to get from input");
    inputRange->setPoint(0, 1);

    outputRange = addPoint2DParameter("Output Range", "The range to remap the value to.");
    outputRange->setPoint(0, 1);
    outputRange->setBounds(0,0,1,1);

    learnMode = addBoolParameter("Learn", "When active, this will automatically set the channel and pitch/number to the next incoming message", false);
    learnMode->isSavable = false;
    learnMode->hideInEditor = true;

    learnRange = addBoolParameter("Learn range", "enable this and move your fader to adapt your input range", false);
    learnRange->isSavable = false;

    addChildControllableContainer(&actionManager);

    showInspectorOnSelect = false;
    updateDisplay();
}

OSCMapping::~OSCMapping()
{
}

void OSCMapping::onContainerParameterChangedInternal(Parameter* p) {
    if (p == learnRange) {
        nextInputReinitRange = learnRange->boolValue();
    }
}

void OSCMapping::updateDisplay()
{
    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}



void OSCMapping::processMessage(const OSCMessage& m)
{
    if (!enabled->boolValue()) return;
    String address = m.getAddressPattern().toString();
    if (address != inputAddress->stringValue()) return;
    float val = 0;
    if (m.size() > 0) {
        val = OSCHelpers::getFloatArg(m[0]);
        val = jmap(val, inputRange->x, inputRange->y,outputRange->x,outputRange->y);
        actionManager.setValueAll(val, "", 0, false);
    }
    else {
        actionManager.setValueAll(1, "", 0, false);
        actionManager.setValueAll(0, "", 0, false);
    }
}

void OSCMapping::handleValue(float value, String origin, int incrementIndex, bool isRelative)
{
    actionManager.setValueAll(value, origin, incrementIndex, isRelative);
    return;
}

/*
InspectableEditor* OSCMapping::getEditorInternal(bool isRoot, Array<Inspectable*> inspectables)
{
    //return new OSCMappingEditor(this, isRoot);
}
*/
