/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "TimingPreset.h"
#include "../Command/CommandSelectionManager.h"
#include "../../Brain.h"
#include "TimingPresetManager.h"

TimingPreset::TimingPreset(var params) :
	BaseItem(params.getProperty("name", "TimingPreset")),
	objectType(params.getProperty("type", "TimingPreset").toString()),
	objectData(params),
	delayMult("Delay multiplicators"),
	fadeMult("Fade multiplicators")
{
	saveAndLoadRecursiveData = true;
	nameCanBeChangedByUser = false;
	canBeDisabled = false;

	itemDataType = "TimingPreset";

	id = addIntParameter("ID", "ID of this timing preset", 1, 1);
	userName = addStringParameter("Name", "Name of this timing preset", "New timing preset");
	updateName();

	useAnotherId = addIntParameter("Use another", "id of another preset to use instead of this one", 0, 0);

	delayFrom = addFloatParameter("Delay", "fade of th first element (in seconds)", 0, 0);
	thruDelay = addBoolParameter("Thru delay", "Do you want to apply multiples delays ?", false);
	delayTo = addFloatParameter("Delay To", "fade of th first element (in seconds)", 0, 0);
	symmetryDelay = addBoolParameter("Delay Symmetry", "Apply this delay in symmetry", false);
	randomizeDelay = addBoolParameter("Delay random", "Randomize the delay values", false);

	fadeFrom = addFloatParameter("Fade", "fade of th first element (in seconds)", 0, 0);
	thruFade = addBoolParameter("Thru fade", "Do you want to apply multiple fades", false);
	fadeTo = addFloatParameter("Fade To", "fade of th first element (in seconds)", 0, 0);
	symmetryFade = addBoolParameter("Fade Symmetry", "Apply this fade in symmetry", false);
	randomizeFade = addBoolParameter("Fade random", "Randomize the fade values", false);

	curveFade.saveAndLoadRecursiveData = true;
	curveFade.setNiceName("Fade curve");
	curveFade.editorIsCollapsed = false;
	curveFade.allowKeysOutside = false;
	curveFade.isSelectable = false;
	curveFade.length->setValue(1);
	curveFade.addKey(0, 0, false);
	curveFade.items[0]->easingType->setValueWithData(Easing::BEZIER);
	curveFade.addKey(1, 1, false);
	curveFade.selectItemWhenCreated = false;
	curveFade.editorCanBeCollapsed = true;

	curveDelayRepart.saveAndLoadRecursiveData = true;
	curveDelayRepart.editorIsCollapsed = false;
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
	curveFadeRepart.editorIsCollapsed = false;
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

	addChildControllableContainer(&delayMult);
	addChildControllableContainer(&fadeMult);

	Brain::getInstance()->registerTimingPreset(this, id->getValue());
	updateDisplay();
}

TimingPreset::~TimingPreset()
{
	Brain::getInstance()->unregisterTimingPreset(this);
}

void TimingPreset::onContainerParameterChangedInternal(Parameter* p) {
	if (p == userName || p == id) {
		updateName();
	}
	if (p == id) {
		Brain::getInstance()->registerTimingPreset(this, id->getValue(), true);
	}
	if (p == thruDelay || p == thruFade) {
		updateDisplay();
	}
	
}

void TimingPreset::updateName() {
	String n = userName->getValue();
	if (parentContainer != nullptr) {
		dynamic_cast<TimingPresetManager*>(parentContainer.get())->reorderItems();
	}
	setNiceName(String((int)id->getValue()) + " - " + n);
}

void TimingPreset::updateDisplay()
{
	bool thd = thruDelay->getValue();
	bool thf = thruFade->getValue();

	delayTo->hideInEditor = !thd;
	symmetryDelay->hideInEditor = !thd;
	randomizeDelay->hideInEditor = !thd;

	fadeTo->hideInEditor = !thf;
	symmetryFade->hideInEditor = !thf;
	randomizeFade->hideInEditor = !thf;

	curveDelayRepart.hideInEditor = !thd;
	curveFadeRepart.hideInEditor = !thf;

	queuedNotifier.addMessage(new ContainerAsyncEvent(ContainerAsyncEvent::ControllableContainerNeedsRebuild, this));
}

