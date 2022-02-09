/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  No

  ==============================================================================
*/

#include "JuceHeader.h"
#include "CommandTiming.h"

CommandTiming::CommandTiming(var params) :
	ControllableContainer("Timing"),
	objectData(params)
{
	saveAndLoadRecursiveData = true;
	editorIsCollapsed = false;

	presetOrValue = addEnumParameter("Timing type", "What kind of timing do you want to apply ?");
	presetOrValue->addOption("Raw Timing", "raw");
	presetOrValue->addOption("Preset", "preset");

	presetId = addIntParameter("ID", "ID of the preset", 1, 1);

	// to add a manager with defined data
	delayFrom = addFloatParameter("Delay", "fade of th first element (in seconds)", 0, 0);
	thruDelay = addBoolParameter("Thru delay", "Do you want to apply multiples delays ?", false);
	delayTo = addFloatParameter("Delay To", "fade of th first element (in seconds)", 0, 0);
	symmetryDelay = addBoolParameter("Delay Symmetry", "Apply this delay in symmetry", false);

	fadeFrom = addFloatParameter("Fade", "fade of th first element (in seconds)", 0, 0);
	thruFade = addBoolParameter("Thru fade", "Do you want to apply multiple fades", false);
	fadeTo = addFloatParameter("Fade To", "fade of th first element (in seconds)", 0, 0);
	symmetryFade = addBoolParameter("Fade Symmetry", "Apply this fade in symmetry", false);

	curveFade.saveAndLoadRecursiveData = true;
	curveFade.setNiceName("Fade curve");
	curveFade.editorIsCollapsed = true;
	curveFade.allowKeysOutside = false;
	curveFade.isSelectable = false;
	curveFade.length->setValue(1);
	curveFade.addKey(0, 0, false);
	curveFade.items[0]->easingType->setValueWithData(Easing::BEZIER);
	curveFade.addKey(1, 1, false);
	curveFade.selectItemWhenCreated = false;
	curveFade.editorCanBeCollapsed = true;

	curveDelayRepart.saveAndLoadRecursiveData = true;
	curveDelayRepart.editorIsCollapsed = true;
	curveDelayRepart.setNiceName("Delay repartition");
	curveDelayRepart.allowKeysOutside = false;
	curveDelayRepart.isSelectable = false;
	curveDelayRepart.length->setValue(1);
	curveDelayRepart.addKey(0, 0, false);
	curveDelayRepart.items[0]->easingType->setValueWithData(Easing::LINEAR);
	curveDelayRepart.addKey(1, 1, false);
	curveDelayRepart.selectItemWhenCreated = false;
	curveDelayRepart.editorCanBeCollapsed = true;

	curveFadeRepart.saveAndLoadRecursiveData = true;
	curveFadeRepart.editorIsCollapsed = true;
	curveFadeRepart.setNiceName("Fade repartition");
	curveFadeRepart.allowKeysOutside = false;
	curveFadeRepart.isSelectable = false;
	curveFadeRepart.length->setValue(1);
	curveFadeRepart.addKey(0, 0, false);
	curveFadeRepart.items[0]->easingType->setValueWithData(Easing::LINEAR);
	curveFadeRepart.addKey(1, 1, false);
	curveFadeRepart.selectItemWhenCreated = false;
	curveFadeRepart.editorCanBeCollapsed = true;

	addChildControllableContainer(&curveFade);
	addChildControllableContainer(&curveDelayRepart);
	addChildControllableContainer(&curveFadeRepart);
	updateDisplay();
}

CommandTiming::~CommandTiming()
{
}

void CommandTiming::updateDisplay()
{
	bool prst = presetOrValue->getValue() == "preset";
	bool thd = thruDelay->getValue();
	bool thf = thruFade->getValue();

	presetId->hideInEditor = !prst;

	// to add a manager with defined data
	delayFrom->hideInEditor = prst;
	thruDelay->hideInEditor = prst;
	delayTo->hideInEditor = prst || !thd;
	symmetryDelay->hideInEditor = prst || !thd;

	fadeFrom->hideInEditor = prst;
	thruFade->hideInEditor = prst;
	fadeTo->hideInEditor = prst || !thf;
	symmetryFade->hideInEditor = prst || !thf;

	curveFade.hideInEditor = prst;
	curveDelayRepart.hideInEditor = prst || !thd;
	curveFadeRepart.hideInEditor = prst || !thf;

	queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

void CommandTiming::parameterValueChanged(Parameter* p) {
	ControllableContainer::parameterValueChanged( p);
	if (p == thruDelay || p == thruFade || p == presetOrValue) {
		updateDisplay();
	}
}

void CommandTiming::afterLoadJSONDataInternal() {
	updateDisplay();
}

