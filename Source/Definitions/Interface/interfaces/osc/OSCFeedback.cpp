/*
  ==============================================================================

    OSCFeedback.cpp
    Created: 12 Oct 2020 11:07:59am
    Author:  bkupe

  ==============================================================================
*/

#include "Definitions/Interface/InterfaceIncludes.h"
#include "UI/VirtualButtons/VirtualButton.h"
#include "UI/VirtualFaders/VirtualFaderButton.h"
#include "UI/VirtualButtons/VirtualButtonGrid.h"
#include "UI/VirtualFaders/VirtualFaderColGrid.h"
#include "OSCFeedback.h"

OSCFeedback::OSCFeedback() :
    BaseItem("OSC Feedback"),
    isValid(false),
    wasInRange(false)
{

    feedbackType = addEnumParameter("Data type", "Do you wanna feedback the text or the value ?");
    feedbackType->addOption("Value", VALUE)
                ->addOption("Text", TEXT);

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

    outputAddress = addStringParameter("Address", "Output address.","");
    outputRange = addPoint2DParameter("OSC Output Range", "The range to remap the value to.");
    outputRange->setPoint(0, 1);

    //inputRange = addPoint2DParameter("Input Range", "The range to get from input",false);
    //inputRange->setBounds(0, 0, 127, 127);
    //inputRange->setPoint(0, 127);
    
    onValue = addIntParameter("On Value", "Value to send when the target is on", 0,0,127);
    offValue  = addIntParameter("Off Value", "Value to send when the target is off", 0,0,127);
    onLoadedValue  = addIntParameter("On Loaded Value", "Value to send when the cuelist is on and has a loaded cue", 0,0,127);
    offLoadedValue  = addIntParameter("Off Loaded Value", "Value to send when the cuelist is off and has a loaded cue", 0,0,127);
    currentCueValue  = addIntParameter("Current Cue Value", "Value to send when the current cue of the cuelist is the one selected in the button", 0,0,127);
    loadedCueValue  = addIntParameter("Loaded Cue Value", "Value to send when the loaded cue of the cuelist is the one selected in the button", 0,0,127);
    isGenericValue  = addIntParameter("Generic Action Value", "Value to send when the target is on", 0,0,127);

    saveAndLoadRecursiveData = true;

    showInspectorOnSelect = false;

    updateDisplay();
}

OSCFeedback::~OSCFeedback()
{
}

void OSCFeedback::updateDisplay() {
    
    FeedbackSource source = feedbackSource->getValueDataAsEnum<FeedbackSource>();
    sourceId->hideInEditor = true ;
    sourcePage->hideInEditor = source == ENCODER || source == GRANDMASTER || source == BLACKOUT;
    onlyIfCurrentPage->hideInEditor = source == ENCODER || source == GRANDMASTER || source == BLACKOUT || sourcePage->intValue()==0;
    sourceCol->hideInEditor = source == ENCODER || source == GRANDMASTER || source == BLACKOUT;
    sourceRow->hideInEditor = source != VBUTTON || source == GRANDMASTER || source == BLACKOUT;
    sourceNumber->hideInEditor = source != VROTARY && source != VABOVEBUTTON && source != VBELOWBUTTON && source != ENCODER;

    bool isButton = source == VBUTTON || source == VABOVEBUTTON || source == VBELOWBUTTON;
    onValue->hideInEditor = !isButton;
    offValue->hideInEditor = !isButton;
    onLoadedValue->hideInEditor = !isButton;
    offLoadedValue->hideInEditor = !isButton;
    currentCueValue->hideInEditor = !isButton;
    loadedCueValue->hideInEditor = !isButton;
    isGenericValue->hideInEditor = !isButton;
    outputRange->hideInEditor = isButton;

    queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

void OSCFeedback::onContainerParameterChangedInternal(Parameter* p)
{
    if (p == feedbackSource || p == sourcePage) {
        updateDisplay();
    }
}

void OSCFeedback::processFeedback(String address, var varValue, String origin, bool logOutput)
{
    if (!enabled->boolValue()) return;
    String localAddress = "";
    FeedbackSource source = feedbackSource->getValueDataAsEnum<FeedbackSource>();
    if (inter == nullptr) {
        inter = dynamic_cast<OSCInterface*>(parentContainer->parentContainer.get());
    }

    if (address != getLocalAdress()) return;
    bool isText = varValue.isString();
    if (isText && feedbackType->getValueDataAsEnum<FeedbackType>() != TEXT) { return; }
    if (!isText && feedbackType->getValueDataAsEnum<FeedbackType>() == TEXT) { return; }

    if (isText) {
        OSCMessage msg(outputAddress->stringValue());
        msg.addArgument(OSCHelpers::varToArgument(varValue.toString(), OSCHelpers::TF));
        inter->sendOSC(msg);
    }
    else {
        OSCMessage msg(outputAddress->stringValue());
        float v = jmap((float)varValue, 0.0f,1.0f,outputRange->x, outputRange->y);
        msg.addArgument(OSCHelpers::varToArgument(v, OSCHelpers::TF));
        inter->sendOSC(msg);
    }
}

String OSCFeedback::getLocalAdress()
{
    String localAddress = "";

    FeedbackSource source = feedbackSource->getValueDataAsEnum<FeedbackSource>();
    if (inter == nullptr) {
        inter = dynamic_cast<OSCInterface*>(parentContainer->parentContainer.get());
    }

    int localPage = sourcePage->intValue();

    bool validPageButton = true;
    bool validPageFader = true;

    if (source == VFADER && validPageFader) {
        localAddress = "/vfader/" + String(localPage) + "/" + String(sourceCol->intValue());
    }
    else if (source == VROTARY && validPageFader) {
        localAddress = "/vrotary/" + String(localPage) + "/" + String(sourceCol->intValue()) + "/" + String(sourceNumber->intValue());
    }
    else if (source == VBUTTON && validPageButton) {
        localAddress = "/vbutton/" + String(localPage) + "/" + String(sourceCol->intValue()) + "/" + String(sourceRow->intValue());
    }
    else if (source == VABOVEBUTTON && validPageFader) {
        localAddress = "/vabovebutton/" + String(localPage) + "/" + String(sourceCol->intValue()) + "/" + String(sourceNumber->intValue());
    }
    else if (source == VBELOWBUTTON && validPageFader) {
        localAddress = "/vbelowbutton/" + String(localPage) + "/" + String(sourceCol->intValue()) + "/" + String(sourceNumber->intValue());
    }
    else if (source == ENCODER) {
        localAddress = "/encoder/" + String(sourceNumber->intValue());
    }
    else if (source == GRANDMASTER) {
        localAddress = "/grandmaster";
    }
    else if (source == BLACKOUT) {
        localAddress = "/blackout";
    }

    return localAddress;
}

