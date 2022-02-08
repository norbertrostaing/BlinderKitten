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

TimingPreset::TimingPreset(var params) :
	BaseItem(params.getProperty("name", "TimingPreset")),
	objectType(params.getProperty("type", "TimingPreset").toString()),
	objectData(params)
{
	saveAndLoadRecursiveData = true;

	itemDataType = "TimingPreset";

	id = addIntParameter("ID", "ID of this device", 1, 1);

	delayFrom = addFloatParameter("Delay", "fade of th first element (in seconds)", 0, 0);
	thruDelay = addBoolParameter("Thru delay", "Do you want to apply multiples delays ?", false);
	delayTo = addFloatParameter("Delay To", "fade of th first element (in seconds)", 0, 0);
	symmetryDelay = addBoolParameter("Delay Symmetry", "Apply this delay in symmetry", false);

	fadeFrom = addFloatParameter("Fade", "fade of th first element (in seconds)", 0, 0);
	thruFade = addBoolParameter("Thru fade", "Do you want to apply multiple fades", false);
	fadeTo = addFloatParameter("Fade To", "fade of th first element (in seconds)", 0, 0);
	symmetryFade = addBoolParameter("Fade Symmetry", "Apply this fade in symmetry", false);

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

	Brain::getInstance()->registerTimingPreset(this, id->getValue());
}

TimingPreset::~TimingPreset()
{
	Brain::getInstance()->unregisterTimingPreset(this);
}

void TimingPreset::parameterValueChanged(Parameter* p) {
	BaseItem::parameterValueChanged(p);
	Logger::writeToLog("changed !");
	if (p == id) {
		Brain::getInstance()->registerTimingPreset(this, id->getValue());
	}
}
