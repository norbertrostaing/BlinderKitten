/*
  ==============================================================================

	Object.cpp
	Created: 26 Sep 2020 10:02:32am
	Author:  bkupe

  ==============================================================================
*/

#include "JuceHeader.h"
#include "CurvePreset.h"
#include "../Command/CommandSelectionManager.h"
#include "../../Brain.h"

CurvePreset::CurvePreset(var params) :
	BaseItem(params.getProperty("name", "CurvePreset")),
	objectType(params.getProperty("type", "CurvePreset").toString()),
	objectData(params)
{
	saveAndLoadRecursiveData = true;

	itemDataType = "CurvePreset";

	id = addIntParameter("ID", "ID of this device", 1, 1);
	selection = new CommandSelectionManager();
	addChildControllableContainer(selection);

	Brain::getInstance()->registerCurvePreset(this, id->getValue());
}

CurvePreset::~CurvePreset()
{
	Brain::getInstance()->unregisterCurvePreset(this);
}

void CurvePreset::parameterValueChanged(Parameter* p) {
	BaseItem::parameterValueChanged(p);
	Logger::writeToLog("changed !");
	if (p == id) {
		Brain::getInstance()->registerCurvePreset(this, id->getValue());
	}
}
