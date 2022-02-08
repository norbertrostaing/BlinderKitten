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
	selection = new CommandSelectionManager();
	addChildControllableContainer(selection);

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
