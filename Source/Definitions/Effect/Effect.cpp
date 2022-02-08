/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "Effect.h"
#include "../Command/CommandSelectionManager.h"
#include "../../Brain.h"

Effect::Effect(var params) :
	BaseItem(params.getProperty("name", "Effect")),
	objectType(params.getProperty("type", "Effect").toString()),
	objectData(params)
{
	saveAndLoadRecursiveData = true;

	itemDataType = "Effect";

	id = addIntParameter("ID", "ID of this device", 1, 1);
	addChildControllableContainer(&selection);

	Brain::getInstance()->registerEffect(this, id->getValue());
}

Effect::~Effect()
{
	Brain::getInstance()->unregisterEffect(this);
}

void Effect::parameterValueChanged(Parameter* p) {
	BaseItem::parameterValueChanged(p);
	Logger::writeToLog("changed !");
	if (p == id) {
		Brain::getInstance()->registerEffect(this, id->getValue());
	}
}
